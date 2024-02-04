#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float exposure;
uniform sampler2D screenTexture;
uniform sampler2D brightFilterTexture;

void main()
{
    vec3 hdrColor  = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor  = texture(brightFilterTexture, TexCoords).rgb;
    hdrColor += bloomColor;
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    FragColor = vec4(result, 1);
} 