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
uniform Texture2D normal_map;
uniform float normalStrength;

float near = 0.1; 
float far  = 100.0;

float saturate(float x)
{
    return clamp(x, 0, 1);
}

vec3 saturate(vec3 x)
{
    return clamp(x, vec3(0,0,0), vec3(1,1,1));
}

void main()
{    
    vec3 normalWS = SampleTexture(normal_map, fs_in.TexCoords).rgb;

    normalWS = normalize(normalWS * 2.0 - 1.0);
    normalWS.xy *= normalStrength;
    mat3 TBN = mat3(fs_in.T, fs_in.B, fs_in.N);
    normalWS = normalize(TBN * normalWS);
    mat4 viewInverse = transpose(inverse(fs_in.ViewMat));
    vec4 normalVS = viewInverse * vec4(normalWS,1);
    normalVS.x = normalVS.x / 2.0 + 0.5;
    normalVS.y = normalVS.y / 2.0 + 0.5;
    vec3 worldTangent = normalize(vec3(fs_in.T));
    vec3 worldBitangent = normalize(vec3(fs_in.B));
    FragColor = vec4(normalVS.xyz, 1.0);
}