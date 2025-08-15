#include <runtime/function/postprocess.h>
#include <runtime/function/render_texture.h>
#include <editor/renderer_window.h>
#include <editor/renderer_console.h>
#include <runtime/function/render_pipeline.h>
#include<runtime/function/scene.h>
#include <random>

PostProcessManager::PostProcessManager(RenderPipeline* _pipeLine, int screen_width, int screen_height)
{
    is_editor = true;
    name = "post process manager";
    atr_ppm = new ATR_PostProcessManager(this);

    read_rt = new RenderTexture(screen_width, screen_height);
    write_rt = new RenderTexture(screen_width, screen_height);
    output_rt = new RenderTexture(screen_width, screen_height);

    default_framebuffer_shader  = new Shader (  FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                                FileSystem::GetContentPath() / "Shader/framebuffer.fs",
                                                true);
    default_framebuffer_shader->LoadShader();

    renderPipeline = _pipeLine;

    InitPostProcess();
}

PostProcessManager::~PostProcessManager() {}

void PostProcessManager::RenderAttribute()
{
    atr_ppm->UI_Implement();
}

void PostProcessManager::InitPostProcess()
{
    // Screen quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void PostProcessManager::ResizeRenderArea(int x, int y)
{
    delete read_rt;
    delete write_rt;
    delete output_rt;

    read_rt = new RenderTexture(x, y);
    write_rt = new RenderTexture(x, y);
    output_rt = new RenderTexture(x, y);

    for (auto postprocess : postprocess_list)
    {
        postprocess->read_rt = read_rt;
        postprocess->write_rt = write_rt;
        postprocess->OnRenderAreaResized(x, y);
    }
}

void PostProcessManager::AddPostProcess(PostProcess* p)
{
    postprocess_list.push_back(p);
    atr_ppm->RefreshAllNode();
}

void PostProcessManager::RemovePostProcess(PostProcess* p)
{
    for (std::vector<PostProcess*>::iterator iter = postprocess_list.begin(); iter != postprocess_list.end(); iter = next(iter))
    {
        if (*iter == p)
        {
            postprocess_list.erase(iter);
        }
    }
    atr_ppm->RefreshAllNode();
}

void PostProcessManager::MoveUpPostProcessOnIndex(int index)
{
    if (index - 1 < 0 || index > postprocess_list.size())
    {
        return;
    }

    auto tmp = postprocess_list[index];
    postprocess_list[index] = postprocess_list[index - 1];
    postprocess_list[index - 1] = tmp;
}

void PostProcessManager::MoveDownPostProcessOnIndex(int index)
{
    if (index < 0 || index + 1 > postprocess_list.size())
    {
        return;
    }

    auto tmp = postprocess_list[index];
    postprocess_list[index] = postprocess_list[index + 1];
    postprocess_list[index + 1] = tmp;
}

void PostProcessManager::ExecutePostProcessList()
{
    if (EditorSettings::UsePostProcess && !EditorSettings::UseWireframeMode )
    {
        glDisable(GL_CULL_FACE);
        for (auto postprocess : postprocess_list)
        {
            if (postprocess->enabled)
            {
                postprocess->Execute(quadVAO);
            }
            
        }
    }
    
    // Draw to default buffer
    // FrameBufferTexture::ClearBufferBinding();
    output_rt->BindFrameBuffer();
    // clear all relevant buffers
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    default_framebuffer_shader->use();
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    if (EditorSettings::UsePreviewGBuffer)
    {
        switch (EditorSettings::CurrentRenderBuffer)
        {
        case EGBuffer::Fragpos :
            glBindTexture(GL_TEXTURE_2D, renderPipeline->fragpos_texture->color_buffer);
            break;

        case EGBuffer::Depth :
            glBindTexture(GL_TEXTURE_2D, renderPipeline->depth_texture->color_buffer);
            break;
        
        case EGBuffer::Normal :
            glBindTexture(GL_TEXTURE_2D, renderPipeline->normal_texture->color_buffer);
            break;

        default:
            glBindTexture(GL_TEXTURE_2D, renderPipeline->fragpos_texture->color_buffer);
            break;
        }
    }
    else{
        glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);	// use the color attachment texture as the texture of the quad plane
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Clear default frameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
}

PostProcess::PostProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled) : read_rt(_rrt), write_rt(_wrt), shader(_shader) , name(_name), enabled(_enabled)
{
    atr_ppn = new ATR_PostProcessNode(this);
}

PostProcess::~PostProcess()
{
    delete atr_ppn;
}

void PostProcess::OnRenderAreaResized(int x, int y)
{
    RendererConsole::GetInstance()->AddWarn("Resize PostProcess: %dx%d", x, y);
}

void PostProcess::BeiginRender()
{
    write_rt->BindFrameBuffer();
    // clear all relevant buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcess::EndRender()
{
    write_rt->SetAsReadTarget();
    read_rt->SetAsRenderTarget();
    glBlitFramebuffer(0, 0, read_rt->width, read_rt->height, 0, 0, write_rt->width, write_rt->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    FrameBufferTexture::ClearBufferBinding(); // Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcess::Execute(unsigned int quad)
{
    // Get camera texture and render to 
    BeiginRender();

    shader->use();
    glBindVertexArray(quad);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    EndRender();
}

BloomProcess::BloomProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled) : PostProcess(_rrt, _wrt, _shader, _name, _enabled)
{
    // Delete the attribute created by PostProcess constructor
    delete atr_ppn;

    atr_ppn = new ATR_BloomProcessNode(this);
    bloom_buffer = new BloomRenderBuffer(_rrt->width, _rrt->height);
    pingpong_buffer[0] = new RenderTexture(_rrt->width, _rrt->height);
    pingpong_buffer[1] = new RenderTexture(_rrt->width, _rrt->height);
    filter_shader = new Shader( FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                FileSystem::GetContentPath() / "Shader/brightFilter.fs",
                                true);
    gaussblur_shader = new Shader( FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                FileSystem::GetContentPath() / "Shader/gaussBlur.fs",
                                true);

    filter_shader->LoadShader();
    gaussblur_shader->LoadShader();
    shader->use();
    glUniform1i(glGetUniformLocation(shader->ID, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(shader->ID, "brightFilterTexture"), 1);
}

BloomProcess::~BloomProcess()
{
    delete atr_ppn;
    delete bloom_buffer;
    delete filter_shader;
}

void BloomProcess::OnRenderAreaResized(int x, int y)
{
    RendererConsole::GetInstance()->AddWarn("Resize Bloom PostProcess: %dx%d", x, y);

    delete bloom_buffer;
    bloom_buffer = new BloomRenderBuffer(x, y);


    delete pingpong_buffer[0];
    delete pingpong_buffer[1];
    pingpong_buffer[0] = new RenderTexture(x, y);
    pingpong_buffer[1] = new RenderTexture(x, y);
    
}

/****************************************
* 1. Get scene color seperate bright color.
* 2. Blur bright color.
* 3. Merge 2 color buffer and output.
*****************************************/
void BloomProcess::Execute(unsigned int quad)
{
    // Get camera color and render to 2 color buffers
    bloom_buffer->BindFrameBuffer();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    filter_shader->use();
    filter_shader->setFloat("threshold", threshold);
    glBindVertexArray(quad);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Blur the bright buffer
    GLboolean horizontal = true, first_iteration = true;
    GLuint amount = 10;
    gaussblur_shader->use();
    for (GLuint i = 0; i < amount; i++)
    {
        pingpong_buffer[horizontal]->BindFrameBuffer(); 
        gaussblur_shader->setBool("horizontal", horizontal);
        glBindTexture(
            GL_TEXTURE_2D, first_iteration ? bloom_buffer->bright_buffer : pingpong_buffer[!horizontal]->color_buffer
        ); 
        
        glBindVertexArray(quad);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindBuffer(GL_FRAMEBUFFER, 0);

    // Merge blur result and screen texture
    BeiginRender();
    shader->use();
    shader->setFloat("exposure", exposure);
    glBindVertexArray(quad);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpong_buffer[1]->color_buffer);
    glActiveTexture(GL_TEXTURE0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    EndRender();
}

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
    return a + f * (b - a);
}

SSAOProcess::SSAOProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled) : PostProcess(_rrt, _wrt, _shader, _name, _enabled)
{
    // Delete the attribute created by PostProcess constructor
    delete atr_ppn;
    // Create custom attribute
    atr_ppn = new ATR_SSAOProcessNode(this);

    ssao_shader = new Shader(   FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                FileSystem::GetContentPath() / "Shader/SSAO.fs",
                                true);
    ssao_shader->LoadShader();
    ssaoTexture = new RenderTexture(_rrt->width, _rrt->height);
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // 随机浮点数，范围0.0 - 1.0
    std::default_random_engine generator;

    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0, 
            randomFloats(generator) * 2.0 - 1.0, 
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0; 
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);  
    }

    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

SSAOProcess::~SSAOProcess()
{
    delete atr_ppn;
}

void SSAOProcess::OnRenderAreaResized(int x, int y)
{
    RendererConsole::GetInstance()->AddWarn("Resize SSAO PostProcess: %dx%d", x, y);

    delete ssaoTexture;
    ssaoTexture = new RenderTexture(x, y);

    normalTexture = Scene::LoadedScene[0]->render_pipeline.normal_texture;
    depthTexture = Scene::LoadedScene[0]->render_pipeline.depth_texture;
    fragPosTexture = Scene::LoadedScene[0]->render_pipeline.fragpos_texture;
}

void SSAOProcess::Execute(unsigned int quad)
{
    ssaoTexture->BindFrameBuffer();
    ssao_shader->use();
    glUniform1i(glGetUniformLocation(ssao_shader->ID, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(ssao_shader->ID, "fragPosTexture"), 1);
    glUniform1i(glGetUniformLocation(ssao_shader->ID, "depthTexture"), 2);
    glUniform1i(glGetUniformLocation(ssao_shader->ID, "normalTexture"), 3);
    glUniform1i(glGetUniformLocation(ssao_shader->ID, "texNoise"), 4);

    Camera* camera = renderWindow->render_camera;
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderWindow->Width() / (float)renderWindow->Height(), 0.1f, 100.0f);
    ssao_shader->setMat4("projection", projection);
    
    for (unsigned int i = 0; i < 64; ++i)
    {
        ssao_shader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }
    ssao_shader->setFloat("radius", radius);
    ssao_shader->setVec2("screenSize", glm::vec2(read_rt->width, read_rt->height));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fragPosTexture->color_buffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexture->color_buffer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalTexture->color_buffer);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(quad);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    BeiginRender();
        shader->use();
        glUniform1i(glGetUniformLocation(shader->ID, "screenTexture"), 0);
        glUniform1i(glGetUniformLocation(shader->ID, "ssaoTexture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ssaoTexture->color_buffer);
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(quad);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

    EndRender();
}

RayMarchingProcess::RayMarchingProcess(RenderTexture *_rrt, RenderTexture *_wrt, Shader *_shader, std::string _name, bool _enabled)
    :PostProcess(_rrt, _wrt, _shader, _name, _enabled)
{
    raycamera = nullptr;
    transform = new Transform();
    glUniform1i(glGetUniformLocation(shader->ID, "screenTexture"), 0);
}

RayMarchingProcess::~RayMarchingProcess() {}

void RayMarchingProcess::Execute(unsigned int quad)
{
    BeiginRender();

        shader->use();
        glm::mat4 model = transform->GetTransformMatrix();
        glm::mat4 view = raycamera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(raycamera->Zoom), (float)read_rt->width / (float)read_rt->height, 0.1f, 10000.0f);
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setFloat("fov", raycamera->Zoom);
        shader->setVec3("eyepos", raycamera->Position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);

        glBindVertexArray(quad);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

    EndRender();
}