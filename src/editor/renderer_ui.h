#pragma once
#include <imgui/imgui.h>
#include <filesystem>
#include <runtime/platform/file_system.h>
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
    void RenderAll          (RendererWindow *window, Scene *scene, float deltaTime);
    void mainUI             (RendererWindow *window, Scene* scene, float deltaTime   );
    void resourceUI         (RendererWindow *window, Scene *scene, float deltaTime   );
    void sceneUI            (RendererWindow *window, Scene* scene, float deltaTime   );
    void detailUI           (RendererWindow *window, Scene *scene, float deltaTime   );
    void RenderPanel        (RendererWindow *window, Scene *scene, float deltaTime   );
    void ImportModelPanel   (RendererWindow *window, float deltaTime                 );
    void ImportShaderPanel  (RendererWindow *window, float deltaTime                 );
    void ImportTexturePanel (RendererWindow *window, float deltaTime                 );
    void FileBrowser        (RendererWindow *window, std::filesystem::path *_path, float deltaTime);
    void SetLayout          ();
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
    bool showRenderpanel            = true;
    bool showDetail                 = true;
    bool showScene                  = true;
    bool showResource               = true;
    std::filesystem::path *file_path;
    std::filesystem::path import_tex_path = FileSystem::GetContentPath();
    std::filesystem::path import_model_path = FileSystem::GetContentPath();
    std::filesystem::path import_shader_path = FileSystem::GetContentPath();

    ImGuiID mainID;

};