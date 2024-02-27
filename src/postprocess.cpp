#include <postprocess.h>
#include <render_texture.h>
#include <renderer_window.h>
#include <renderer_console.h>


PostProcessManager::PostProcessManager(int screen_width, int screen_height, DepthTexture* _depthTexture) : depthTexture(_depthTexture)
{
    is_editor = true;
    name = "post process manager";
    atr_ppm = new ATR_PostProcessManager(this);

    read_rt = new RenderTexture(screen_width, screen_height);
    write_rt = new RenderTexture(screen_width, screen_height);
    default_framebuffer_shader = new Shader (   FileSystem::GetContentPath() / "Shader/framebuffer.vs",
                                                FileSystem::GetContentPath() / "Shader/framebuffer.fs",
                                                true);
    default_framebuffer_shader->LoadShader();
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

    read_rt = new RenderTexture(x, y);
    write_rt = new RenderTexture(x, y);

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
    glDisable(GL_CULL_FACE);
    for (auto postprocess : postprocess_list)
    {
        if (postprocess->enabled)
        {
            postprocess->Execute(quadVAO);
        }
        
    }
    
    // Draw to default buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // clear all relevant buffers
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    default_framebuffer_shader->use();
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

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
    RendererConsole::GetInstance()->AddWarn("Resize PostProcess: %dx%d", x, y);

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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    EndRender();
}