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
} fs_in;

uniform sampler2D albedo_map;
uniform vec3 color;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{    
    vec3 objectColor = vec3(1, 1, 1);
    float specularStrength = 0.2;


    vec4 albedo = texture(albedo_map, fs_in.TexCoords);
    vec3 normal = fs_in.Normal;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * fs_in.LightColor * albedo.xyz;
  	
    // diffuse 
    vec3 lightDir = normalize(fs_in.LightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedo.xyz * color;

    // specular
    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal); 
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 5);
    vec3 specular = specularStrength * spec * fs_in.LightColor;

    FragColor = vec4((ambient + diffuse + specular) * objectColor,1.0);
    // FragColor = vec4(normal,1);
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    // FragColor = vec4(vec3(depth), 1.0);
}