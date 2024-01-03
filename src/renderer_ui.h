#pragma once
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

class RendererWindow;
class SceneObject;
class Scene;

class renderer_ui
{
public:
    renderer_ui();
    ~renderer_ui();
    void setup              (GLFWwindow *window                     );
    void RenderAll          (RendererWindow *window, Scene *scene   );
    void mainUI             (RendererWindow *window                 );
    void resourceUI         (RendererWindow *window, Scene *scene   );
    void sceneUI            (RendererWindow *window, Scene* scene   );
    void detailUI           (RendererWindow *window, Scene *scene   );
    void ImportModelPanel   (RendererWindow *window                 );
    void ImportShaderPanel  (RendererWindow *window                 );
    void ImportTexturePanel (RendererWindow *window                 );
    void shutdown();
    static bool isFocusOnUI();

private:
    SceneObject *selected;
    bool showImportModelPanel       = false;
    bool showImportShaderPanel      = false;
    bool showImportTexturePanel     = false;

};