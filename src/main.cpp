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

// Create camera
Camera camera(glm::vec3(0.0f, 20.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90, -40);

int main()
{
    RendererWindow main_window(&camera, "Tiny-Renderer(v1.1.0)", WindowSize(window_width, window_height));
    // Create scene
    Scene* scene = new Scene(&main_window);
    main_window.AttatchObserver(&scene->render_pipeline);

    // Load Resources
    // ------------------------------------------------------------------------------------------------------------------------------
    Texture2D *normalTex    = new Texture2D(FileSystem::FileSystem::GetContentPath() / "Textures/normal.png", ETexType::RGBA, true);
    Texture2D *wallTex      = new Texture2D(FileSystem::FileSystem::GetContentPath() / "Textures/wall.jpg");
    Texture2D *whiteTex     = new Texture2D(FileSystem::FileSystem::GetContentPath() / "Textures/white.png", ETexType::SRGBA, true);
    Texture2D *folder_ico   = new Texture2D(FileSystem::FileSystem::GetEditorPath() / "ico/folder_ico.png", ETexType::SRGBA, true);
    Texture2D *file_ico     = new Texture2D(FileSystem::FileSystem::GetEditorPath() / "ico/file_ico.png", ETexType::SRGBA, true);
    EditorContent::editor_tex.insert({"default_normal_map", normalTex});
    EditorContent::editor_tex.insert({"folder_ico", folder_ico});
    EditorContent::editor_tex.insert({"file_ico", file_ico});
    EditorContent::editor_tex.insert({"default_tex", whiteTex});

    Model *M_sphere     = new Model(FileSystem::FileSystem::GetContentPath() / "Models/sphere.fbx");
    Model *M_cube       = new Model(FileSystem::FileSystem::GetContentPath() / "Models/cube.fbx");
    // build and compile our shader program
    // ------------------------------------
    Shader *default_shader  = new Shader(   FileSystem::GetContentPath() / "Shader/default.vs",
                                            FileSystem::GetContentPath() / "Shader/default.fs",
                                            true);
    Shader *color_shader    = new Shader(   FileSystem::GetContentPath() / "Shader/color.vs",
                                            FileSystem::GetContentPath() / "Shader/color.fs",
                                            true);
    Shader *model_shader    = new Shader(   FileSystem::GetContentPath() / "Shader/default.vs",
                                            FileSystem::GetContentPath() / "Shader/model.fs",
                                            true);
    Shader *PBR_shader      = new Shader(   FileSystem::GetContentPath() / "Shader/default.vs",
                                            FileSystem::GetContentPath() / "Shader/PBR.fs",
                                            true);
    Shader *gamma_correcting_shader   = new Shader(   FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                            FileSystem::GetContentPath() / "Shader/gamma_correcting_frame.fs",
                                            true);
    Shader *inverse_shader  = new Shader(   FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                            FileSystem::GetContentPath() / "Shader/inverse_color.fs",
                                            true);
    Shader *blur_shader     = new Shader(   FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                            FileSystem::GetContentPath() / "Shader/blur.fs",
                                            true);

    Shader *bloom_shader     = new Shader(  FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                            FileSystem::GetContentPath() / "Shader/bloom.fs",
                                            true);

    default_shader->LoadShader();
    color_shader->LoadShader();
    model_shader->LoadShader();
    PBR_shader->LoadShader();
    gamma_correcting_shader->LoadShader();
    inverse_shader->LoadShader();
    blur_shader->LoadShader();
    bloom_shader->LoadShader();

    // Create a post process manager
    PostProcessManager* ppm = new PostProcessManager(main_window.Width(), main_window.Height(), scene->render_pipeline.depth_texture);
    scene->RegisterSceneObject(ppm);
    // Assign postprocess manager to scene's renderer pipeline
    scene->render_pipeline.postprocess_manager = ppm;

    // Add a gamma correct post process
    ppm->AddPostProcess( ppm->CreatePostProcess<PostProcess>( gamma_correcting_shader, "Gamma correction" ));
    // Post process for test
    ppm->AddPostProcess( ppm->CreatePostProcess<PostProcess>( inverse_shader, "inverse", false));
    ppm->AddPostProcess( ppm->CreatePostProcess<PostProcess>( blur_shader, "Blur", false ));
    ppm->AddPostProcess( ppm->CreatePostProcess<BloomProcess>( bloom_shader, "Bloom", false ));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    // Render loop
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
        // Render
        // ------
        scene->render_pipeline.clear_color = main_window.clear_color;
        scene->RenderScene();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // --------------------------------- ----------------------------------------------
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI
        main_window.imgui->RenderAll(&main_window, scene);
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(main_window.Window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    main_window.terminate_window();

    return 0;
}