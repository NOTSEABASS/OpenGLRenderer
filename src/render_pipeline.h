#pragma once

#include <map>

class SceneModel;
class SceneLight;
class Camera;
class RendererWindow;
class PostProcess;

class RenderPipeline
{
public:
    void EnqueueRenderQueue     ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( unsigned int id                       );
    SceneModel* GetRenderModel  ( unsigned int id                       );
    void Render                 ( RendererWindow* window, Camera *camera);

    float *clear_color;
    SceneLight* global_light;

private:
    std::map<unsigned int, SceneModel *> ModelQueueForRender;
    const double Pi = 3.1415926;
};