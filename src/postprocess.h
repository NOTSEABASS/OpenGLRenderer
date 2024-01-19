#pragma once
#include <glad/glad.h>
#include <shader.h>
#include <list>
#include <scene_object.h>
#include <attributes.h>

class RenderTexture;
class RendererWindow;
class Shader;
class DepthTexture;

class PostProcess
{
public:
    
    RenderTexture   *read_rt;
    RenderTexture   *write_rt;
    Shader          *shader;
    bool            enabled = true;
    std::string     name;
    ATR_PostProcessNode* atr_ppn;

    PostProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled = true);
    ~PostProcess();

    void BeiginRender();
    void EndRender();

    /******************************************
    * Draw result of all post process,
    * Should call after all post process node.
    *******************************************/
    void Execute(unsigned int quad);
};

class PostProcessManager : public SceneObject
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
    std::list<PostProcess*> postprocess_list;
    ATR_PostProcessManager* atr_ppm;
    void InitPostProcess();

public:
    PostProcessManager(int screen_width, int screen_height,  DepthTexture* _depthTexture);
    ~PostProcessManager();

    PostProcess* CreatePostProcess(Shader * shader, std::string _name, bool default_enabled = true);

    void AddPostProcess(PostProcess* p);
    void RemovePostProcess(PostProcess* p);
    void ExecutePostProcessList();
    /******************************************
    * Delete the origin rt and create a new one
    * to fit the given window size.
    * Should call after resizing the window.
    *******************************************/
    void ResizeRenderArea(int x, int y);
    unsigned int GetRenderQuad() { return quadVAO; }

    void RenderAttribute() override;

    Shader* default_framebuffer_shader;
    
    RenderTexture   *read_rt;
    RenderTexture   *write_rt;

    DepthTexture* depthTexture;
};

