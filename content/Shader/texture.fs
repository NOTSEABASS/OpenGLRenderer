#version 330 core
out vec4 FragColor;

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

// texture sampler
uniform sampler2D shadowMap;
uniform Texture2D albedo_map;

void main()
{
	FragColor = SampleTexture(albedo_map, fs_in.TexCoords);
	// FragColor = vec4(1,1,1,1); 
}