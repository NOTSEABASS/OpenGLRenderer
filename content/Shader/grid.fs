#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 nearPoint;
in vec3 farPoint;
in mat4 ViewMat;
in mat4 ProjMat;

uniform vec3 cameraPos;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

float Grid(vec2 uv)
{
    vec2 derivative = fwidth(uv);
    uv = fract(uv - 0.5); //中心对齐
    uv = abs(uv - 0.5);
    uv = uv / derivative;
    float min_value = min(uv.x, uv.y);
    float grid = 1.0 - min(min_value, 1.0);
    return grid;
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = ProjMat * ViewMat * vec4(pos.xyz, 1.0);
    return clip_space_pos.w;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = ProjMat * ViewMat * vec4(pos.xyz, 1.0);
    // map to 0-1
    return (clip_space_pos.z / clip_space_pos.w) * 0.5 + 0.5;
}

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    float scene_depth = texture(depthTexture, TexCoords).r;
    float ty = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + ty * (farPoint - nearPoint);
    float ground  = step(0, ty);
    float fromOrigin = abs(cameraPos.y);
    vec2 uv = fragPos3D.xz;
    float grid_depth = computeLinearDepth(fragPos3D);
    float fading = max(0.0, 1.0 - grid_depth / (1000 * fromOrigin / 20));
    float smallGrid     = Grid(uv) * mix(1, 0, min(1.0, fromOrigin / 100));
    float middleGrid    = Grid(uv * 0.1) * mix(1, 0, min(1.0, fromOrigin / 300)); //同理
    float largeGrid     = Grid(uv * 0.01) * mix(1, 0, min(1.0, fromOrigin / 800)); //同理
    float grid = smallGrid + middleGrid + largeGrid;
    vec4 grid_res = vec4(0.2,0.2,0.2, ground * grid * fading * 0.2);
    gl_FragDepth = computeDepth(fragPos3D);
    FragColor = grid_res;
} 