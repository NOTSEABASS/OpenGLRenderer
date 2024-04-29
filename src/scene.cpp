#include <renderer_window.h>
#include <scene_object.h>
#include <camera.h>
#include <model.h>
#include <scene.h>
#include <shader.h>

std::map<unsigned int, Scene*> Scene::LoadedScene;
unsigned int Scene::next_scene_id = 0;

Scene::Scene(RendererWindow *_window) : window(_window), render_pipeline(RenderPipeline(_window))
{
    // Create a default light
    RegisterGlobalLight(new SceneLight("Global Light", true));
    scene_id = next_scene_id++;
    LoadedScene[scene_id] = this;
}

Scene::~Scene() {}
void Scene::RegisterSceneObject(SceneObject *object)            { scene_object_list.push_back(object);     }
void Scene::RenderScene()                                       { render_pipeline.Render();   }

void Scene::RegisterGlobalLight( SceneLight *light)
{
    RegisterSceneObject(light);
    render_pipeline.global_light = light;
}

void Scene::InstanceFromModel(Model *model, std::string name)
{
    SceneModel *scene_model = new SceneModel(model, name);
    RegisterSceneObject(scene_model);
    render_pipeline.EnqueueRenderQueue(scene_model);
}

void Scene::RemoveSceneObjectAtIndex(int index)
{
    if (index >= scene_object_list.size())
    {
        return;
    }
    auto it = scene_object_list.begin() + index;
    SceneObject* target_so = *it;
    render_pipeline.RemoveFromRenderQueue(target_so->id);
    scene_object_list.erase(it);
    delete target_so;
}