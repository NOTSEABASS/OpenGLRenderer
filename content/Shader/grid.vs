#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 nearPoint;
out vec3 farPoint;
out mat4 ViewMat;
out mat4 ProjMat;

vec3 UnprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  inverse(view) * inverse(projection) * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    nearPoint = UnprojectPoint(aPos.x, aPos.y, -1.0).xyz;// unprojecting on the near plane
    farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0).xyz;// unprojecting on the far plane
    TexCoords = aTexCoords;
    ViewMat = view;
    ProjMat = projection;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  