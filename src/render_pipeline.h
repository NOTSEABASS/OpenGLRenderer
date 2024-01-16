#pragma once

#include <map>
#include <renderer_window.h>

class SceneModel;
class SceneLight;
class Camera;
class RendererWindow;
class PostProcessManager;

class RenderPipeline : public IOnWindowSizeChanged
{
public:
    RenderPipeline();
    ~RenderPipeline();
    void EnqueueRenderQueue     ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( unsigned int id                       );
    SceneModel* GetRenderModel  ( unsigned int id                       );
    void Render                 ( RendererWindow* window, Camera *camera);
    void OnWindowSizeChanged    (int width, int height                  ) override;

    float *clear_color;
    SceneLight* global_light;
    PostProcessManager *postprocess_manager = nullptr;

private:
    std::map<unsigned int, SceneModel *> ModelQueueForRender;

};