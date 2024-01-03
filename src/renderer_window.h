#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera.h>
#include <iostream>
#include <string>
#include <map>

class renderer_ui;

class RendererWindow
{
public:
    RendererWindow(Camera *camera, std::string name);
    RendererWindow(Camera *camera, std::string name, unsigned int _width, unsigned int _height);
    void terminate_window();
    Camera          *render_camera;
    renderer_ui     *imgui;
    std::string     window_name;
    unsigned int    width           = 800;
    unsigned int    height          = 600;
private:
    GLFWwindow *init_window();

private:
public:
    static std::map<GLFWwindow *, Camera *> window_camera_map;
    GLFWwindow                              *Window;
    float                                   *clear_color;
};