#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D ssaoTexture;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, TexCoords + offset).r;
        }
    }
    float ao = result / 16.0;

    vec4 col = texture(screenTexture, TexCoords);
    FragColor = vec4(col.r * ao,col.g * ao,col.b * ao, 1);
} 