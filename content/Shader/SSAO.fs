#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;

void main()
{
    vec4 col = texture(screenTexture, TexCoords).rgba;
    float sampleDepth = texture(depthTexture, TexCoords).r;
    vec3 sampleNormal = texture(normalTexture, TexCoords).rgb;

    float ao = 0;
    for(int x = -10; x < 10; x++)
    {
        for (int y = -10; y < 10; y++)
        {
            vec2 offset;
            offset = vec2(x * 0.001,y * 0.001);
            float offset_depth = texture(depthTexture, TexCoords + offset).r;
            if (offset_depth < sampleDepth)
            {
                ao += 1 * (1-(sampleDepth - offset_depth));
            }
        }
    }
    ao /= 400;
    ao = ao * ao;
    ao = 1-ao;
    FragColor = vec4(col.r * ao,col.g * ao,col.b * ao ,1);
} 