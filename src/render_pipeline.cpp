#include <render_pipeline.h>
#include <scene_object.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <camera.h>
#include <shader.h>
#include <renderer_window.h>
#include <postprocess.h>
#include <render_texture.h>
#include <editor_settings.h>
#include <model.h>
#include <shader.h>
#include <glm/gtx/matrix_decompose.hpp>

#include <gizmos.h>

void RenderPipeline::EnqueueRenderQueue(SceneModel *model)     { ModelQueueForRender.insert({model->id, model});   }
void RenderPipeline::RemoveFromRenderQueue(unsigned int id)    { ModelQueueForRender.erase(id);                    }

RenderPipeline::RenderPipeline(RendererWindow* _window) : window(_window) 
{
    depth_texture = new DepthTexture(window->Width(), window->Height());
    shadow_map = new DepthTexture(shadow_map_setting.shadow_map_size, shadow_map_setting.shadow_map_size);
    depth_shader = new Shader(  FileSystem::GetContentPath() / "Shader/depth.vs",
                                FileSystem::GetContentPath() / "Shader/depth.fs",
                                true);
    depth_shader->LoadShader();
}
RenderPipeline::~RenderPipeline() {}

SceneModel *RenderPipeline::GetRenderModel(unsigned int id)
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

void RenderPipeline::OnWindowSizeChanged(int width, int height)
{
    postprocess_manager->ResizeRenderArea(width, height);

    // Resize depth texture as well
    delete depth_texture;
    depth_texture = new DepthTexture(width, height);
}

/*********************
* Shadow Pass
**********************/
void RenderPipeline::ProcessShadowPass()
{
    glViewport(0, 0, shadow_map_setting.shadow_map_size, shadow_map_setting.shadow_map_size);
    glEnable(GL_DEPTH_TEST);
    shadow_map->BindFrameBuffer();
    glClear(GL_DEPTH_BUFFER_BIT);
    GLfloat near_plane = 1.0f, far_plane = 10000.0f;
    float sdm_size = shadow_map_setting.shadow_distance;
    glm::mat4 light_projection = glm::ortho(-sdm_size, sdm_size, -sdm_size, sdm_size, near_plane, far_plane);
    Transform* light_transform = global_light->atr_transform->transform;
    // glm::mat4 light_view = glm::lookAt(light_transform->Position(), light_transform->Position() - light_transform->GetFront(), light_transform->Position() + light_transform->GetUp());
    auto camera_pos = window->render_camera->Position;
    glm::mat4 light_view = glm::lookAt(-light_transform->GetFront() * glm::vec3(50) + camera_pos, glm::vec3(0,0,0) + camera_pos, glm::vec3(0,1,0));
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    for (std::map<unsigned int, SceneModel *>::iterator it = ModelQueueForRender.begin(); it != ModelQueueForRender.end(); it++)
    {
        SceneModel *sm = it->second;
        depth_shader->use();
        Transform *transform = sm->atr_transform->transform;
        glm::mat4 m = glm::mat4(1.0f);
        m = transform->GetTransformMatrix();
        depth_shader->setMat4("model", m);                      // M
        depth_shader->setMat4("view", light_view);              // V    
        depth_shader->setMat4("projection", light_projection);  // P

        
        for (int i = 0; i < sm->meshRenderers.size(); i++)
        {
            if (sm->meshRenderers[i]->cast_shadow)
            {
                // Draw without any material
                sm->meshRenderers[i]->PureDraw();
            }
        }
    }
    // glDisable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
}

/*********************
* Z-Pre Pass
**********************/
void RenderPipeline::ProcessZPrePass()
{
    glViewport(0, 0, window->Width(), window->Height());
    glEnable(GL_DEPTH_TEST);
    depth_texture->BindFrameBuffer();
    glClear(GL_DEPTH_BUFFER_BIT);
    // view/projection transformations
    Camera* camera = window->render_camera;
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)window->Width() / (float)window->Height(), 0.1f, 10000.0f);
    glm::mat4 view = camera->GetViewMatrix();

    for (std::map<unsigned int, SceneModel *>::iterator it = ModelQueueForRender.begin(); it != ModelQueueForRender.end(); it++)
    {
        SceneModel *sm = it->second;
        depth_shader->use();
        Transform *transform = sm->atr_transform->transform;
        glm::mat4 m = glm::mat4(1.0f);
        m = transform->GetTransformMatrix();
        depth_shader->setMat4("model", m);                // M
        depth_shader->setMat4("view", view);              // V    
        depth_shader->setMat4("projection", projection);  // P

        
        for (int i = 0; i < sm->meshRenderers.size(); i++)
        {
            // Draw without any material
            sm->meshRenderers[i]->PureDraw();
        }
    }
}

/*********************
* Color Pass
**********************/
void RenderPipeline::ProcessColorPass()
{
    glViewport(0, 0, window->Width(), window->Height());
    // view/projection transformations
    Camera* camera = window->render_camera;
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)window->Width() / (float)window->Height(), 0.1f, 10000.0f);
    glm::mat4 view = camera->GetViewMatrix();

    GLfloat near_plane = 1.0f, far_plane = 10000.0f;
    float sdm_size = shadow_map_setting.shadow_distance;
    glm::mat4 light_projection = glm::ortho(-sdm_size, sdm_size, -sdm_size, sdm_size, near_plane, far_plane);
    Transform* light_transform = global_light->atr_transform->transform;
    // glm::mat4 light_view = glm::lookAt(light_transform->Position(), light_transform->Position() - light_transform->GetFront(), light_transform->Position() + light_transform->GetUp());
    auto camera_pos = window->render_camera->Position;
    glm::mat4 light_view = glm::lookAt(-light_transform->GetFront() * glm::vec3(50) + camera_pos, glm::vec3(0,0,0) + camera_pos, glm::vec3(0,1,0));
    // Render Scene (Color Pass)
    for (std::map<unsigned int, SceneModel *>::iterator it = ModelQueueForRender.begin(); it != ModelQueueForRender.end(); it++)
    {
        SceneModel *sm = it->second;

        Material* prev_mat = nullptr;
        for (auto mr : sm->meshRenderers)
        {
            Material* mat = mr->material;
            if (mat == prev_mat)
            {
                continue;
            }
            prev_mat = mat;
            Shader* shader;
            if (EditorSettings::UsePolygonMode || !mat->shader->IsValid())
            {
                shader = Shader::LoadedShaders["default.fs"];
            }
            else
            {
                shader = mat->shader;
            }
            shader->use();
            // Render the loaded model
            Transform *transform = sm->atr_transform->transform;
            glm::mat4 m = glm::mat4(1.0f);
            m = transform->GetTransformMatrix();
            shader->setMat4("model", m);                // M
            shader->setMat4("view", view);              // V    
            shader->setMat4("projection", projection);  // P
            shader->setVec3("viewPos", camera->Position);
            shader->setMat4("light_view", light_view);
            shader->setMat4("light_projection", light_projection);

            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(shader->ID, "shadowMap"), 0);
            glBindTexture(GL_TEXTURE_2D, shadow_map->color_buffer);

            if (global_light != nullptr)
            {
                glm::vec3 front = global_light->atr_transform->transform->GetFront();
                shader->setVec3("lightDir", front);
                glm::vec3 lightColor = global_light->GetLightColor();
                shader->setVec3("lightColor", lightColor);
            }
            else
            {
                shader->setVec3("lightDir", glm::vec3(1, 1, 1));
                shader->setVec3("lightColor", glm::vec3(1, 0, 0));
            }
        }
        sm->DrawSceneModel();
    }
}

/*********************
* Render Gizmos
**********************/
void RenderPipeline::RenderGizmos()
{
    glDisable(GL_DEPTH_TEST);
    Camera* camera = window->render_camera;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)window->Width() / (float)window->Height(), 0.1f, 10000.0f);
    Shader::LoadedShaders["color.fs"]->use();
    Shader::LoadedShaders["color.fs"]->setMat4("model", model);
    Shader::LoadedShaders["color.fs"]->setMat4("view", view);
    Shader::LoadedShaders["color.fs"]->setMat4("projection", projection);

    // Draw coordinate axis
    glLineWidth(4);
    for (std::map<unsigned int, SceneModel *>::iterator it = ModelQueueForRender.begin(); it != ModelQueueForRender.end(); it++)
    {
        SceneModel *sm = it->second;
        if (sm->is_selected)
        {
            
            Transform* transform = sm->atr_transform->transform; 
            GLine front(transform->Position(), transform->Position() + transform->GetFront());
            front.color = glm::vec3(0,0,1);
            front.DrawInGlobal();
            GLine right(transform->Position(), transform->Position() + transform->GetRight());
            right.color = glm::vec3(1,0,0);
            right.DrawInGlobal();
            GLine up(transform->Position(), transform->Position() + transform->GetUp());
            up.color = glm::vec3(0,1,0);
            up.DrawInGlobal();
        }
    }

    // Draw light debug cube
    glLineWidth(2);
    if (global_light->is_selected)
    {
        float r = 0.2;
        GCube light_cube(0.2);
        light_cube.color = global_light->GetLightColor();
        light_cube.transform = *global_light->atr_transform->transform;
        light_cube.Draw();

        // GLine front(glm::vec3(0), glm::vec3(0,0,2));
        GLine front(light_cube.transform.Position(), (light_cube.transform.Position() + glm::vec3(2) * light_cube.transform.GetFront()));
        front.color = global_light->GetLightColor();
        front.DrawInGlobal();
    }

    // Draw a grid
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GGrid grid;
    Shader::LoadedShaders["grid.fs"]->use();
    Shader::LoadedShaders["grid.fs"]->setMat4("view", view);
    Shader::LoadedShaders["grid.fs"]->setMat4("projection", projection);
    Shader::LoadedShaders["grid.fs"]->setVec3("cameraPos", camera->Position);
    grid.Draw();
    glDisable(GL_BLEND);
}

/****************************************************************
* Render order is decide by create order by now.
* If there's a requirement to render model with alpha
* we need to sort models by distance to camera. 
* All models is rendered without alpha clip or alpha blend now.
*****************************************************************/
void RenderPipeline::Render()
{
    // Draw shadow pass
    ProcessShadowPass();

    // Z-PrePass
    ProcessZPrePass();


    // Pre Render Setting
    if (EditorSettings::UsePostProcess && !EditorSettings::UsePolygonMode && postprocess_manager != nullptr)
    {
        postprocess_manager->read_rt->BindFrameBuffer();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glClearColor(clear_color[0],clear_color[1],clear_color[2],1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (EditorSettings::UsePolygonMode)
    {
        glLineWidth(0.05);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Draw color pass
    ProcessColorPass();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Draw Gizmos
    if (EditorSettings::DrawGizmos)
    {
        RenderGizmos();
    }

    // PostProcess
    if (EditorSettings::UsePostProcess && !EditorSettings::UsePolygonMode && postprocess_manager != nullptr)
    {
        postprocess_manager->ExecutePostProcessList();
    }
}