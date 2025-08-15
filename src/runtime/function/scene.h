#pragma once
#include <vector>
#include <string>
#include <runtime/function/render_pipeline.h>
class SceneLight;
class SceneObject;
class RendererWindow;
class Shader;
class Camera;
class Model;
class Scene
{
public:
    std::vector<SceneObject *>  scene_object_list;
    RenderPipeline              render_pipeline;
    static std::map<unsigned int, Scene*> LoadedScene;
    static unsigned int next_scene_id;
public:
    Scene(RendererWindow *window);
    ~Scene();
    unsigned int scene_id;
    void RegisterSceneObject        ( SceneObject *object                    );
    void RegisterGlobalLight        ( SceneLight *light                      );
    void InstanceFromModel          ( Model *model, std::string name         );
    void RemoveSceneObjectAtIndex   ( int index                              );
    void RenderScene                ();

    RendererWindow *window;
};