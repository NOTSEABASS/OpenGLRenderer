#pragma once
#include <glad/glad.h>
#include <shader.h>
#include <list>
#include <scene_object.h>
#include <attributes.h>
#include <type_traits>

class RenderTexture;
class BloomRenderBuffer;
class RendererWindow;
class Shader;
class DepthTexture;

/************************************************************
* To call PostProcessManager::CreatePostProcess correctly,
* all class derived from PostProcess should implement a 
* constructor same as PostProcess, which has 5 input parameters.
************************************************************/
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
    virtual ~PostProcess();

    virtual void OnRenderAreaResized(int x, int y);
    virtual void BeiginRender();
    virtual void EndRender();

    /******************************************
    * Draw result of all post process,
    * Should call after all post process node.
    *******************************************/
    virtual void Execute(unsigned int quad);
};

class BloomProcess : public PostProcess
{
public:
    BloomProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled = true);
    ~BloomProcess();

    virtual void OnRenderAreaResized(int x, int y);
    // virtual void BeiginRender();
    // virtual void EndRender();
    virtual void Execute(unsigned int quad);

    float exposure = 1;
    float threshold = 1;

private:
    Shader* filter_shader;
    Shader* gaussblur_shader;
    BloomRenderBuffer *bloom_buffer;
    RenderTexture *pingpong_buffer[2];
};

class SSAOProcess : public PostProcess
{
public:
    SSAOProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled = true);
    ~SSAOProcess();

    virtual void Execute(unsigned int quad);

    RenderTexture* normalTexture;
    DepthTexture* depthTexture;
};

/************************************************************
* Post process manager:
* Will process all post process in a user-defined order.
*************************************************************/
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
    std::vector<PostProcess*> postprocess_list;
    ATR_PostProcessManager* atr_ppm;
    void InitPostProcess();

public:
    friend class ATR_PostProcessManager;
    
    PostProcessManager(int screen_width, int screen_height, DepthTexture* _depthTexture, RenderTexture* _normalTexture);
    ~PostProcessManager();

    template<class T>
    typename std::enable_if_t<std::is_base_of_v<PostProcess, T>, T>* CreatePostProcess(Shader * shader, std::string name, bool default_enabled = true)
    {
        return new T(read_rt, write_rt, shader, name, default_enabled);
    }

    void AddPostProcess(PostProcess* p);
    void RemovePostProcess(PostProcess* p);
    void MoveUpPostProcessOnIndex(int index);
    void MoveDownPostProcessOnIndex(int index);
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
    RenderTexture* normalTexture;
};

