#include <renderer_window.h>
#include <scene_object.h>
#include <camera.h>
#include <model.h>
#include <scene.h>
#include <shader.h>

void Scene::RegisterSceneObject(SceneObject *object)            { scene_object_list.push_back(object);  }
void Scene::RenderScene(RendererWindow *window, Camera *camera) { renderQueue.Render(window, camera);   }

void Scene::InstanceFromModel(Model *model, std::string name)
{
    SceneModel *sceneModel = new SceneModel(model, name);
    RegisterSceneObject(sceneModel);
    renderQueue.EnqueueRenderQueue(sceneModel);
}

void Scene::RemoveSceneObjectAtIndex(int index)
{
    if (index >= scene_object_list.size())
    {
        return;
    }
    auto it = scene_object_list.begin() + index;
    SceneObject* target_so = *it;
    renderQueue.RemoveFromRenderQueue(target_so->id);
    scene_object_list.erase(it);
    delete target_so;
}