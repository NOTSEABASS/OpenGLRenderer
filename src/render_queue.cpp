#include <render_queue.h>
#include <scene_object.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <camera.h>
#include <shader.h>
#include <renderer_window.h>
#include <model.h>

void RenderQueue::EnqueueRenderQueue(SceneModel *model)     { ModelQueueForRender.insert({model->id, model});   }
void RenderQueue::RemoveFromRenderQueue(unsigned int id)    { ModelQueueForRender.erase(id);                    }

SceneModel *RenderQueue::GetRenderModel(unsigned int id)
{
    if (ModelQueueForRender.find(id) != ModelQueueForRender.end())
    {
        return ModelQueueForRender[id];
    }
    else
    {
        return nullptr;
    }
}

void RenderQueue::Render(RendererWindow *window, Camera *camera)
{
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)window->width / (float)window->height, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    for (std::map<unsigned int, SceneModel *>::iterator it = ModelQueueForRender.begin(); it != ModelQueueForRender.end(); it++)
    {
        // a SceneModel's meshes share one transform
        // so we will set the model matrix only once while render a SceneModel
        SceneModel *sm = it->second;
        Material* mat = sm->meshRenderers[0]->material;
        mat->shader->use();
        mat->shader->setMat4("projection", projection);
        mat->shader->setMat4("view", view);
        mat->shader->setVec3("viewPos", camera->Position);

        // render the loaded model
        ATR_Transform *transform = sm->transform;
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, transform->Position); // translate it down so it's at the center of the scene
        m = glm::rotate(m, glm::radians(transform->Rotation.r), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(transform->Rotation.g), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(transform->Rotation.b), glm::vec3(0, 0, 1));
        m = glm::scale(m, transform->Scale); // it's a bit too big for our scene, so scale it down
        mat->shader->setMat4("model", m);

        sm->DrawAllMeshRenderer();
    }
}