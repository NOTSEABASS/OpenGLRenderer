#pragma once

#include <map>
#include <renderer_window.h>

class SceneModel;
class SceneLight;
class Camera;
class Shader;
class DepthTexture;
class RenderTexture;
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
    Camera* GetDefaultCamera    () { return window->render_camera; };

    struct ShadowMapSetting
    {
        float shadow_map_size = 4096;
        float shadow_distance = 50;
    } shadow_map_setting;

    float *clear_color;
    SceneLight* global_light;
    RenderTexture* normal_texture;
    PostProcessManager *postprocess_manager = nullptr;
    RenderTexture* fragpos_texture;
    DepthTexture* depth_texture;
    DepthTexture* shadow_map;

private:
    std::map<unsigned int, SceneModel *> ModelQueueForRender;
    RendererWindow *window;
    Shader* depth_shader;
    Shader* grid_shader;
    Shader* normal_shader;
    Shader* fragpos_shader;

    void ProcessZPrePass        ();
    void ProcessFragposPass     ();
    void ProcessShadowPass      ();
    void ProcessNormalPass      ();
    void ProcessColorPass       ();
    void RenderGizmos           ();

};