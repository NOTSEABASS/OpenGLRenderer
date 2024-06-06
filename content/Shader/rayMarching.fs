#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float fov;
uniform vec3 eyepos;

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;



void main()
{
    // vec4 col = texture(screenTexture, TexCoords).rgba;
    // float ratio = 1920.0/1080.0;
    // vec3 eye = vec3(0.0, 0.0, length(eyepos));
    // float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    
    // if (dist > MAX_DIST - EPSILON) {
        // Didn't hit anything
        // FragColor = col;
		// return;
    // }
    
    //The closest point on the surface to the eyepoint along the view ray
    // vec3 p = eye + dist * dir;
    
    // vec3 K_a = vec3(0.2, 0.2, 0.2);
    // vec3 K_d = vec3(0.7, 0.2, 0.2);
    // vec3 K_s = vec3(1.0, 1.0, 1.0);
    // float shininess = 10.0;
    
    // vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, eye);

    // Volume Test
    // vec4 worldpos = GetWorldSpacePosition(1, TexCoords);
    // vec3 raypos = eyepos;
    // vec3 dir = normalize(worldpos.xyz - raypos);
    // float cloud = CloudRayMarching(eyepos, dir); 
    // if (cloud <= EPSILON)
    // {
    //     FragColor = col;
    //     return;
    // }
    // vec3 color =  vec3(cloud, cloud, cloud) + col.xyz;
    FragColor = vec4(TexCoords.xy,1,1);
} 