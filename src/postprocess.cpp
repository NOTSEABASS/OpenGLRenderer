#include <postprocess.h>
#include <render_texture.h>
#include <renderer_window.h>


PostProcessManager::PostProcessManager(int screen_width, int screen_height, DepthTexture* _depthTexture) : depthTexture(_depthTexture)
{
    is_editor = true;
    name = "post process manager";
    atr_ppm = new ATR_PostProcessManager();

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

    for (auto p : postprocess_list)
    {
        p->atr_ppn->UI_Implement();
    }
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
    }
}

PostProcess* PostProcessManager::CreatePostProcess(Shader * shader, std::string _name, bool default_enabled)
{
    return new PostProcess(read_rt, write_rt, shader, _name, default_enabled);
}

void PostProcessManager::AddPostProcess(PostProcess* p)
{
    postprocess_list.push_back(p);
}

void PostProcessManager::RemovePostProcess(PostProcess* p)
{
    postprocess_list.remove(p);
}

void PostProcessManager::ExecutePostProcessList()
{
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcess::Execute(unsigned int quad)
{
    
    BeiginRender();

    shader->use();
    glBindVertexArray(quad);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glBindTexture(GL_TEXTURE_2D, read_rt->color_buffer);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    EndRender();
}