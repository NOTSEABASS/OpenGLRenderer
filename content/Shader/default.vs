#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 LightPos;
    vec3 ViewPos;
    vec3 T;
    vec3 B;
    vec3 N;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = normalMatrix * aNormal;  
    vs_out.TexCoords = aTexCoords;
    vec3 T = normalize(normalMatrix * aTangent).xyz;
    vec3 N = normalize(normalMatrix * aNormal).xyz;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    vs_out.T = T;
    vs_out.B = B;
    vs_out.N = N;
    vs_out.LightPos = vec3(100,100,100);
    vs_out.ViewPos = viewPos;
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}