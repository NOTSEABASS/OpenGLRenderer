#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D ssaoTexture;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    const int blurSize = 2;
    for (int x = -blurSize; x < blurSize; ++x) 
    {
        for (int y = -blurSize; y < blurSize; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, TexCoords + offset).r;
        }
    }
    float ao = result / pow(blurSize * 2, 2);

    vec4 col = texture(screenTexture, TexCoords);
    FragColor = vec4(col.r * ao,col.g * ao,col.b * ao, 1);
} 