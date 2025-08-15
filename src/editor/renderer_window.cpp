#include <runtime/function/input_management.h>
#include <editor/renderer_window.h>
#include <runtime/function/postprocess.h>
#include <editor/renderer_ui.h>
#include <iostream>

std::map<GLFWwindow *, Camera *> RendererWindow::window_camera_map;
std::map<GLFWwindow* , RendererWindow *> RendererWindow::w_rw_map;
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    WindowSize new_size(width, height);
    RendererWindow::w_rw_map[window]->SetWindowSize(new_size);
    
}

// mouse and keyboard callback functions(broadcast to InputInfo)
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    InputInfo::GetInstance()->mouse_pos_x = xpos;
    InputInfo::GetInstance()->mouse_pos_y = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (renderer_ui::isFocusOnUI())
    {
        return;
    }
    InputInfo::GetInstance()->mouse_scroll_x = xoffset;
    InputInfo::GetInstance()->mouse_scroll_y = yoffset;
    RendererWindow::window_camera_map[window]->ProcessMouseScroll(xoffset, yoffset);
}

void mouse_click_callback(GLFWwindow *window, int button, int state, int mod)
{
    if (renderer_ui::isFocusOnUI())
    {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS)
    {
        InputInfo::GetInstance()->mouse_button_right = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_RELEASE)
    {
        InputInfo::GetInstance()->mouse_button_right = false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS)
    {
        InputInfo::GetInstance()->mouse_button_left = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_RELEASE)
    {
        InputInfo::GetInstance()->mouse_button_left = false;
    }
}

void RendererWindow::SetWindowSize(WindowSize size)
{
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    cur_window_size = size;
    glViewport(0, 0, size.width, size.height);
    glfwSetWindowSize(Window, size.width + renderer_ui::rightside + renderer_ui::leftside, size.height + renderer_ui::bottomside);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    BroadcastWindowSizeChanged();
}

void RendererWindow::AttatchObserver(IOnWindowSizeChanged* observer)
{
    observers.push_back(observer);
}

void RendererWindow::DetatchObserver(IOnWindowSizeChanged* observer)
{
    observers.remove(observer);
}

void RendererWindow::BroadcastWindowSizeChanged()
{
    for (auto o : observers)
    {
        o->OnWindowSizeChanged(cur_window_size.width, cur_window_size.height);
    }
}



RendererWindow::RendererWindow(Camera *camera, std::string name) : render_camera(camera), window_name(name)
{
    Window = init_window();
    // imgui = new renderer_ui();
    // imgui->setup(Window);
    window_camera_map.insert({Window, render_camera});
    w_rw_map.insert({Window, this});
    clear_color = new float[3]{0};
}

RendererWindow::RendererWindow(Camera *camera, std::string name, WindowSize size) : render_camera(camera), window_name(name), cur_window_size(size)
{
    Window = init_window();
    // imgui = new renderer_ui();
    // imgui->setup(Window);
    window_camera_map.insert({Window, render_camera});
    w_rw_map.insert({Window, this});
    clear_color = new float[3]{0};
}

GLFWwindow *RendererWindow::init_window()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(cur_window_size.width + renderer_ui::leftside + renderer_ui::rightside, cur_window_size.height + renderer_ui::bottomside, window_name.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    // glfwSetMouseButtonCallback(window, mouse_click_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}

void RendererWindow::terminate_window()
{
    std::cout << "Terminate window." << std::endl;
    // imgui->shutdown();
    glfwTerminate();
}