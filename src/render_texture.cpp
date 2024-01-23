#include <glad/glad.h>
#include <render_texture.h>
#include <iostream>

FrameBufferTexture::FrameBufferTexture(int _width, int _height) : width(_width), height(_height) {}
FrameBufferTexture::~FrameBufferTexture() {}

void FrameBufferTexture::SetAsRenderTarget()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
}

void FrameBufferTexture::SetAsReadTarget()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
}

void FrameBufferTexture::BindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

RenderTexture::RenderTexture(int _width, int _height) : FrameBufferTexture(_width, _height)
{
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    // HDR
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);
    // LDR
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    CreateFrameBuffer(_width, _height);
    std::cout << "Create Render Texture: " << _width << "x" << _height << std::endl;
}

RenderTexture::~RenderTexture()
{   
    std::cout << "Delete RenderTexture" << std::endl;
    glDeleteTextures(1, &color_buffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteFramebuffers(1, &framebuffer);
}

/*******************************************************************
* Create a frame buffer and bind RenderTexture and render buffer.
********************************************************************/
void RenderTexture::CreateFrameBuffer(int _width, int _height)
{
    // Create frame buffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Bind color attatchment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);

    // Create and bind render buffer object (depth and stencil)
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthTexture::DepthTexture(int _width, int _height) : FrameBufferTexture(_width, _height)
{
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    CreateFrameBuffer(_width, _height);
    std::cout << "Create Depth Texture: " << _width << "x" << _height << std::endl;
}

DepthTexture::~DepthTexture()
{   
    std::cout << "Delete Depth Texture" << std::endl;
    glDeleteTextures(1, &color_buffer);
    glDeleteFramebuffers(1, &framebuffer);
}

/*******************************************************************
* Create a frame buffer and bind DepthTexture.
********************************************************************/
void DepthTexture::CreateFrameBuffer(int _width, int _height)
{
    // Create frame buffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Bind color attatchment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, color_buffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}