#pragma once
#include <vector>
#include <string>
#include <render_queue.h>
class SceneObject;
class RendererWindow;
class Shader;
class Camera;
class Model;
class Scene
{
public:
    std::vector<SceneObject *>  scene_object_list;
    RenderQueue                 renderQueue;

public:
    void RegisterSceneObject        (SceneObject *object                    );
    void InstanceFromModel          (Model *model, std::string name         );
    void RemoveSceneObjectAtIndex   (int index                              );
    void RenderScene                (RendererWindow *window, Camera *camera );
};