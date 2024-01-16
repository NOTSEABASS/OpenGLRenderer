#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    float gamma = 2.2;
    vec3 fragColor = pow(col, vec3(1.0/gamma));
    FragColor = vec4(fragColor, 1.0);
} 