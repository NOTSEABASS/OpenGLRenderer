#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <editor_settings.h>
#include <camera.h>
#include <iostream>
#include <string>
#include <map>

class renderer_ui;
class PostProcess;

class RendererWindow
{
public:
    RendererWindow(Camera *camera, std::string name);
    RendererWindow(Camera *camera, std::string name, WindowSize size);
    void terminate_window();
    unsigned int Width()    { return cur_window_size.width;     }
    unsigned int Height()   { return cur_window_size.height;    }
    void SetWindowSize(WindowSize size);
    Camera          *render_camera;
    renderer_ui     *imgui;
    std::string     window_name;
    WindowSize      cur_window_size = EditorSettings::window_size_list[0];
private:
    GLFWwindow *init_window();

public:
    static std::map<GLFWwindow *, Camera *>             window_camera_map;
    static std::map<GLFWwindow* , RendererWindow *>     w_rw_map;
    GLFWwindow                                          *Window;
    float                                               *clear_color;
    PostProcess                                         *postprocess = nullptr;
};