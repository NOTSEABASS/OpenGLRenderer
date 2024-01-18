#version 330 core
out vec4 FragColor;

uniform vec3 color;
void main()
{
    FragColor = vec4(color, 1.0); // 将向量的四个分量全部设置为1.0
}