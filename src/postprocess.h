#pragma once
#include <glad/glad.h>
#include <shader.h>

class RenderTexture;
class RendererWindow;

class PostProcess
{
private:
    int width;
    int height;
    unsigned int    rbo             =   0;
    unsigned int quadVAO, quadVBO;
    const float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    void InitPostProcess();
    void CreateFrameBuffer();

public:
    unsigned int    framebuffer     =   0;
    GLuint          depthMap;
    RenderTexture   *rt;
    Shader          *shader;

    PostProcess(RendererWindow window, Shader *_shader);
    ~PostProcess();

    /******************************************
    * Draw result of all post process,
    * Should call after all post process node.
    *******************************************/
    void DrawPostProcessResult();

    /******************************************
    * Delete the origin rt and create a new one
    * to fit the given window size.
    * Should call after resizing the window.
    *******************************************/
    void ResizeRenderArea(int x, int y);


};