#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <renderer_window.h>
#include <input_management.h>
#include <file_system.h>
#include <renderer_ui.h>
#include <scene_object.h>
#include <material.h>
#include <model.h>
#include <scene.h>
#include <shader.h>
#include <editor_content.h>
#include <render_texture.h>
#include <editor_settings.h>
#include <postprocess.h>
#include <iostream>
#include <vector>

#define window_width    1920
#define window_height   1080

const double Pi = 3.1415926;

bool focused        = false;
float currentFrame  = 0.0f;     // 当前帧与上一帧的时间差
float lastFrame     = 0.0f;     // 上一帧的时间
float deltaTime     = 0.0f;

// camera
Camera camera(glm::vec3(0.0f, 20.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90, -40);
Scene scene;

int main()
{
    RendererWindow main_window(&camera, "Renderer", WindowSize(window_width, window_height));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    Texture2D *normalTex    = new Texture2D(FileSystem::FileSystem::GetContentPath() / "Textures/normal.png", ETexType::RGBA, true);
    Texture2D *wallTex      = new Texture2D(FileSystem::FileSystem::GetContentPath() / "Textures/wall.jpg");
    Texture2D *whiteTex     = new Texture2D(FileSystem::FileSystem::GetContentPath() / "Textures/white.png", ETexType::SRGBA, true);
    Texture2D *folder_ico   = new Texture2D(FileSystem::FileSystem::GetEditorPath() / "ico/folder_ico.png", ETexType::SRGBA, true);
    Texture2D *file_ico     = new Texture2D(FileSystem::FileSystem::GetEditorPath() / "ico/file_ico.png", ETexType::SRGBA, true);
    EditorContent::editor_tex.insert({"default_normal_map", normalTex});
    EditorContent::editor_tex.insert({"folder_ico", folder_ico});
    EditorContent::editor_tex.insert({"file_ico", file_ico});
    EditorContent::editor_tex.insert({"default_tex", whiteTex});
    // load models
    // -----------
    Model *M_nanosuit   = new Model(FileSystem::FileSystem::GetContentPath() / "Models/nanosuit/nanosuit.obj");
    Model *M_bunny      = new Model(FileSystem::FileSystem::GetContentPath() / "Models/bunny.fbx");
    Model *M_sphere     = new Model(FileSystem::FileSystem::GetContentPath() / "Models/sphere.fbx");
    Model *M_cube       = new Model(FileSystem::FileSystem::GetContentPath() / "Models/cube.fbx");
    // build and compile our shader program
    // ------------------------------------
    Shader *default_shader  = new Shader(   FileSystem::GetContentPath() / "Shader/default.vs",
                                            FileSystem::GetContentPath() / "Shader/default.fs",
                                            true);
    Shader *lighting_shader = new Shader(   FileSystem::GetContentPath() / "Shader/color.vs",
                                            FileSystem::GetContentPath() / "Shader/color.fs");
    Shader *model_shader    = new Shader(   FileSystem::GetContentPath() / "Shader/default.vs",
                                            FileSystem::GetContentPath() / "Shader/model.fs",
                                            true);
    Shader *PBR_shader      = new Shader(   FileSystem::GetContentPath() / "Shader/default.vs",
                                            FileSystem::GetContentPath() / "Shader/PBR.fs",
                                            true);
    Shader *screen_shader   = new Shader(   FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                            FileSystem::GetContentPath() / "Shader/framebuffer.fs",
                                            true);

    default_shader->LoadShader();
    lighting_shader->LoadShader();
    model_shader->LoadShader();
    PBR_shader->LoadShader();
    screen_shader->LoadShader();

    // Create a postprocess
    PostProcess *postprocess = new PostProcess(main_window, screen_shader);
    main_window.postprocess = postprocess;

    SceneLight global_light("Global Light", true);
    scene.RegisterSceneObject(&global_light);
    scene.renderPipeline.global_light = &global_light;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(main_window.Window))
    {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        InputInfo::GetInstance()->Update();
        Input::processInput(main_window.Window, camera, deltaTime);

        focused = InputInfo::GetInstance()->mouse_button_right;
        if (focused)
        {
            camera.ProcessMouseMovement(InputInfo::GetInstance()->mouse_offset_x,
                                        InputInfo::GetInstance()->mouse_offset_y);
        }
        // render
        // ------
        scene.renderPipeline.clear_color = main_window.clear_color;
        scene.RenderScene(&main_window, &camera);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // --------------------------------- ----------------------------------------------
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        main_window.imgui->RenderAll(&main_window, &scene);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(main_window.Window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    main_window.terminate_window();

    return 0;
}