#pragma once
#include <imgui/imgui.h>
#include <filesystem>
#include <file_system.h>
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
    void mainUI             (RendererWindow *window, Scene* scene   );
    void resourceUI         (RendererWindow *window, Scene *scene   );
    void sceneUI            (RendererWindow *window, Scene* scene   );
    void detailUI           (RendererWindow *window, Scene *scene   );
    void RenderPanel        (RendererWindow *window, Scene *scene   );
    void ImportModelPanel   (RendererWindow *window                 );
    void ImportShaderPanel  (RendererWindow *window                 );
    void ImportTexturePanel (RendererWindow *window                 );
    void FileBrowser        (RendererWindow *window, std::filesystem::path *_path);
    void shutdown();
    static bool isFocusOnUI();

    const static float leftside;
    const static float bottomside;
    const static float rightside;
    
private:
    SceneObject *selected;
    bool showImportModelPanel       = false;
    bool showImportShaderPanel      = false;
    bool showImportTexturePanel     = false;
    bool showFileBrowser            = false;
    bool showConsole                = false;
    std::filesystem::path *file_path;
    std::filesystem::path import_tex_path = FileSystem::GetContentPath();
    std::filesystem::path import_model_path = FileSystem::GetContentPath();
    std::filesystem::path import_shader_path = FileSystem::GetContentPath();

};