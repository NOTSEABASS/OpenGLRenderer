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


uniform sampler2D shadowMap;
uniform Texture2D albedo_map;
uniform vec3 color;

float near = 0.1; 
float far  = 100.0; 

void main()
{    
    float specularStrength = 0.2;


    vec4 albedo = SampleTexture(albedo_map, fs_in.TexCoords);
    vec3 normal = fs_in.Normal;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * fs_in.LightColor * albedo.xyz * color;
  	
    // diffuse 
    vec3 lightDir = -normalize(fs_in.LightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedo.xyz * color;

    // specular
    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal); 
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 5);
    vec3 specular = specularStrength * spec * fs_in.LightColor;

    FragColor = vec4((ambient + diffuse + specular),1.0);
}