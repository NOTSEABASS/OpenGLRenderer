#version 330 core
layout (location = 0) out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 LightDir;
    vec3 LightColor;
    vec3 ViewPos;
    vec3 T;
    vec3 B;
    vec3 N;
    vec4 FragPosLightSpace;
    mat4 ViewMat;
} fs_in;

struct Texture2D
{
    sampler2D texture;
    vec2 tilling;
    vec2 offset;
};

vec4 SampleTexture(Texture2D tex, vec2 uv)
{
    return texture(tex.texture, vec2(uv.xy * tex.tilling) + tex.offset);
}

uniform sampler2D shadowMap;
uniform Texture2D albedo_map;
uniform Texture2D normal_map;
uniform Texture2D metal_map;
uniform Texture2D roughness_map;
uniform Texture2D ao_map;
uniform Texture2D spec_map;

uniform vec3 color;
uniform vec3 specularColor;
uniform float normalStrength;
uniform float aoStrength;
uniform float roughnessStrength;
uniform float metalStrength;
uniform float shadowStrength;

float near = 0.1; 
float far  = 100.0;


struct PBRLightingInfo
{
    vec3 Kd;
    float Ks;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

float saturate(float x)
{
    return clamp(x, 0, 1);
}

vec3 saturate(vec3 x)
{
    return clamp(x, vec3(0,0,0), vec3(1,1,1));
}

float GGX(float cos, float k)
{
    return cos / (k + (1 - k) * cos);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow((1.0 - cosTheta) * 1, 5.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1 - F0) * pow((1 - cosTheta), 5.0);
}

PBRLightingInfo CalculatePBRLighting(vec3 n, vec3 l, vec3 v, vec3 t, vec3 b,
    vec3 cspec, vec3 albedo, vec3 lightColor,
    float m, float r, vec3 ambient, float ao)
{
    float pi = 3.1415926;
    vec3 h = normalize(l + v);
    float NdotL = dot(n, l);
    float NdotH = dot(n, h);
    float NdotV = dot(n, v);

    // D
    float a = pow(r, 2);
    float a2 = max(pow(a, 2), 0.000001);
    float D_n = a2 / (pi * pow((pow(NdotH, 2) * (a2 - 1) + 1), 3));

    //F
    vec3 F0 = vec3(0.04, 0.04, 0.04);
    F0 = mix(F0, albedo, m);
    vec3 F = fresnelSchlick(max(dot(h, v), 0), F0);

    //G
    float k = pow((a + 1), 2) / 8;
    float G = GGX(saturate(NdotL), k) * GGX(saturate(NdotV), k);

    float Ks = F.x;
    vec3 Kd = vec3(saturate(1 - Ks) * (1 - m), saturate(1 - Ks) * (1 - m), saturate(1 - Ks) * (1 - m));
    vec3 lambert = albedo * lightColor;

    vec3 specular = (D_n * F * G) / max((4 * NdotV * NdotL), 0.001);

    PBRLightingInfo pbr;
    pbr.Kd = Kd;
    pbr.Ks = Ks;
    pbr.diffuse = lambert;
    pbr.specular = cspec * specular * lightColor;
    pbr.ambient = ambient * albedo * ao;
    return pbr;
}

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

vec3 GetPBRLightingResult(PBRLightingInfo PBR, float NdotL, float shadow)
{
    return (((PBR.Kd) * PBR.diffuse + PBR.specular) * NdotL + PBR.ambient) * (1 - shadow) + PBR.ambient * shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = max(0.0001 * (1.0 - dot(normal, lightDir)), 0.00001);
    // float bias = 0.00001;
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow * shadowStrength;
}

void main()
{    
    vec4 albedo = SampleTexture(albedo_map, fs_in.TexCoords);
    vec4 metallic = SampleTexture(metal_map, fs_in.TexCoords) * metalStrength;
    vec4 ao = SampleTexture(ao_map, fs_in.TexCoords) * aoStrength;
    vec3 specColor = SampleTexture(spec_map, fs_in.TexCoords).xyz * specularColor;
    vec4 roughness = SampleTexture(roughness_map, fs_in.TexCoords) * roughnessStrength;
    vec3 normalWS = SampleTexture(normal_map, fs_in.TexCoords).rgb;

    normalWS = normalize(normalWS * 2.0 - 1.0);
    normalWS.xy *= normalStrength;
    mat3 TBN = mat3(fs_in.T, fs_in.B, fs_in.N);
    normalWS = normalize(TBN * normalWS);
    vec3 worldTangent = normalize(vec3(fs_in.T));
    vec3 worldBitangent = normalize(vec3(fs_in.B));
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * fs_in.LightColor * albedo.xyz;
    vec3 lightDir = normalize(-fs_in.LightDir);
    float NdotL = max(dot(normalWS, lightDir), 0.0);
    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normalWS); 
    vec3 halfwayDir = normalize(lightDir + viewDir); 


    PBRLightingInfo PBR = CalculatePBRLighting(normalWS, lightDir, viewDir, worldTangent, worldBitangent, 
                                                            specColor, color * albedo.xyz, fs_in.LightColor, metallic.x, roughness.x, 
                                                            ambient, ao.x);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normalWS, lightDir);
    vec3 midres = GetPBRLightingResult(PBR, NdotL, shadow);
    FragColor = vec4(midres.rgb, 1.0);
}