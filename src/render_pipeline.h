#pragma once

#include <map>
#include <renderer_window.h>

class SceneModel;
class SceneLight;
class Camera;
class Shader;
class DepthTexture;
class RendererWindow;
class PostProcessManager;

class RenderPipeline : public IOnWindowSizeChanged
{
public:
    RenderPipeline( RendererWindow* _window);
    ~RenderPipeline();
    void EnqueueRenderQueue     ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( unsigned int id                       );
    SceneModel* GetRenderModel  ( unsigned int id                       );
    void Render                 ();
    void OnWindowSizeChanged    (int width, int height                  ) override;

    float *clear_color;
    SceneLight* global_light;
    PostProcessManager *postprocess_manager = nullptr;
    DepthTexture* depthTexture;

private:
    std::map<unsigned int, SceneModel *> ModelQueueForRender;
    RendererWindow *window;
    Shader* depth_shader;

    void ProcessZPrePass        ();
    void ProcessColorPass       ();
    void RenderGizmos           ();

};