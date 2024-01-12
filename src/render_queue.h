#pragma once

#include <map>

class SceneModel;
class Camera;
class RendererWindow;
class PostProcess;

class RenderQueue
{
public:
    void EnqueueRenderQueue     ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( SceneModel *model                     );
    void RemoveFromRenderQueue  ( unsigned int id                       );
    SceneModel* GetRenderModel  ( unsigned int id                       );
    void Render                 ( RendererWindow* window, Camera *camera);

    float *clear_color;

private:
    std::map<unsigned int, SceneModel *> ModelQueueForRender;
};