#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D fragPosTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 10000.0f; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}

void main()
{
    vec4 col = texture(screenTexture, TexCoords).rgba;
    float sampleDepth = texture(depthTexture, TexCoords).r;
    vec3 sampleNormal = texture(normalTexture, TexCoords).rgb;
    sampleNormal = normalize(sampleNormal * 2.0 - 1.0);
    vec3 samplefragPos = texture(fragPosTexture, TexCoords).rgb;

    vec3 fragPos = samplefragPos;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
    vec3 tangent = normalize(randomVec - sampleNormal * dot(randomVec, sampleNormal));
    vec3 bitangent = cross(sampleNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, sampleNormal);

    float occlusion = 0.0;
    int kernelSize = 64;
    float radius = 1;
    for(int i = 0; i < kernelSize; ++i)
    {
        // 获取样本位置
        vec3 sample = TBN * samples[i]; // 切线->观察空间
        sample = fragPos + sample * radius; 
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // 观察->裁剪空间
        offset.xyz /= offset.w; // 透视划分
        offset.xyz = offset.xyz * 0.5 + 0.5; // 变换到0.0 - 1.0的值域

        float sampleDepth = -LinearizeDepth(texture(depthTexture, offset.xy).r);
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + 0.05 ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = vec4(occlusion,occlusion,occlusion,1); 
} 