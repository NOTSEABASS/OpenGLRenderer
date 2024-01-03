#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 LightPos;
    vec3 ViewPos;
    vec3 T;
    vec3 B;
    vec3 N;
} fs_in;

uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metal_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;
uniform sampler2D spec_map;
uniform vec3 color;
uniform float normalStrength;
uniform float aoStrength;
uniform float roughnessStrength;
uniform float metalStrength;
uniform float specularStrength;

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
    pbr.specular = saturate(cspec * specular);
    pbr.ambient = ambient * albedo * ao;
    return pbr;
}

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

vec3 GetPBRLightingResult(PBRLightingInfo PBR, float NdotL)
{
    return ((PBR.Kd) * PBR.diffuse + PBR.specular) * NdotL + PBR.ambient;
}


void main()
{    
    vec3 lightColor = vec3(1, 1, 1);

    vec4 albedo = texture(albedo_map, fs_in.TexCoords);
    vec4 metallic = texture(metal_map, fs_in.TexCoords) * metalStrength;
    vec4 ao = texture(ao_map, fs_in.TexCoords) * aoStrength;
    vec4 specColor = texture(spec_map, fs_in.TexCoords) * specularStrength;
    vec4 roughness = texture(roughness_map, fs_in.TexCoords) * roughnessStrength;
    vec3 normalWS = texture(normal_map, fs_in.TexCoords).rgb;
    normalWS = normalize(normalWS * 2.0 - 1.0);
    normalWS.xy *= normalStrength;
    mat3 TBN = mat3(fs_in.T, fs_in.B, fs_in.N);
    normalWS = normalize(TBN * normalWS);
    vec3 worldTangent = normalize(vec3(fs_in.T));
    vec3 worldBitangent = normalize(vec3(fs_in.B));
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor * albedo.xyz;
    vec3 lightDir = normalize(fs_in.LightPos - fs_in.FragPos);
    float NdotL = max(dot(normalWS, lightDir), 0.0);
    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normalWS); 
    vec3 halfwayDir = normalize(lightDir + viewDir); 


    PBRLightingInfo PBR = CalculatePBRLighting(normalWS, lightDir, viewDir, worldTangent, worldBitangent, 
                                                            specColor.xyz, color * albedo.xyz, lightColor, metallic.x, roughness.x, 
                                                            ambient, ao.x);
    vec3 midres = GetPBRLightingResult(PBR, NdotL);
    FragColor = vec4(midres,1.0);
    // FragColor = vec4(specular,1);
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    // FragColor = vec4(vec3(depth), 1.0);
}