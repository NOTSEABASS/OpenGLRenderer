#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera.h>
#include <singleton_util.h>
#include <renderer_window.h>
#include <renderer_console.h>

class InputInfo : public Singleton<InputInfo>
{
public:
    float   mouse_pos_x;
    float   mouse_pos_y;
    float   mouse_offset_x;
    float   mouse_offset_y;
    float   mouse_scroll_x;
    float   mouse_scroll_y;
    bool    mouse_button_left;
    bool    mouse_button_right;

private:
    static  InputInfo *inputInfo_instance;
    float   mouse_last_x;
    float   mouse_last_y;

public:
    InputInfo() ;
    void Update();
};

namespace Input
{
    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    static void processInputGlobal(GLFWwindow *window, Camera &camera, float deltaTime)
    {
        // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        //     glfwSetWindowShouldClose(window, true);

        // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        //     camera.ProcessKeyboard(FORWARD, deltaTime);
        // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        //     camera.ProcessKeyboard(BACKWARD, deltaTime);
        // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        //     camera.ProcessKeyboard(LEFT, deltaTime);
        // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        //     camera.ProcessKeyboard(RIGHT, deltaTime);
        // if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        //     camera.ProcessKeyboard(UP, deltaTime);
        // if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        //     camera.ProcessKeyboard(DOWN, deltaTime);

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)){
            glfwSetWindowShouldClose(window, true);
        }
    }

    static void processInputRenderPanel(GLFWwindow *window, Camera &camera, float deltaTime){
        if (ImGui::IsKeyDown(ImGuiKey_MouseRight)){
            if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_UpArrow)){
                camera.ProcessKeyboard(FORWARD, deltaTime);
            }
            if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow)){
                camera.ProcessKeyboard(LEFT, deltaTime);
            }
            if (ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_DownArrow)){
                camera.ProcessKeyboard(BACKWARD, deltaTime);
            }
            if (ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow)){
                camera.ProcessKeyboard(RIGHT, deltaTime);
            }
            if (ImGui::IsKeyDown(ImGuiKey_Q)){
                camera.ProcessKeyboard(DOWN, deltaTime);
            }
            if (ImGui::IsKeyDown(ImGuiKey_E)){
                camera.ProcessKeyboard(UP, deltaTime);
            }
        }
    }
}