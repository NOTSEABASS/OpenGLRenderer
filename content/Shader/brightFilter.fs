#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform float threshold;
uniform sampler2D screenTexture;

void main()
{
    vec4 col = texture(screenTexture, TexCoords).rgba;
    FragColor = col;

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > threshold)
        BrightColor = vec4(FragColor.rgb, 1.0);
} 