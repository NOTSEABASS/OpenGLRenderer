#pragma once
#include <glad/glad.h>
#include <shader.h>
#include <list>
#include <singleton_util.h>

class RenderTexture;
class RendererWindow;
class Shader;

class PostProcess
{
public:
    
    RenderTexture   *read_rt;
    RenderTexture   *write_rt;
    Shader          *shader;

    PostProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader);
    ~PostProcess();

    void BeiginRender();
    void EndRender();

    /******************************************
    * Draw result of all post process,
    * Should call after all post process node.
    *******************************************/
    void Execute(unsigned int quad);
};

class PostProcessManager
{
private:
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
    std::list<PostProcess*> postprocess_queue;

    void InitPostProcess();

public:
    PostProcessManager(int screen_width, int screen_height);
    ~PostProcessManager();

    PostProcess* CreatePostProcess(Shader * shader);

    void AddPostProcess(PostProcess* p);
    void RemovePostProcess(PostProcess* p);
    void ExecutePostProcessQueue();
     /******************************************
    * Delete the origin rt and create a new one
    * to fit the given window size.
    * Should call after resizing the window.
    *******************************************/
    void ResizeRenderArea(int x, int y);
    unsigned int GetRenderQuad() { return quadVAO; }

    Shader* default_framebuffer_shader;
    
    RenderTexture   *read_rt;
    RenderTexture   *write_rt;
};

