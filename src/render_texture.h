#pragma once
#include <string>

class FrameBufferTexture
{
public:
    unsigned int    color_buffer;
    int             width;
    int             height;
    std::string     name;

    FrameBufferTexture(int _width, int _height);
    virtual ~FrameBufferTexture();
    void SetAsRenderTarget();
    void SetAsReadTarget();
    void BindFrameBuffer();

protected:
    virtual void CreateFrameBuffer(int _width, int _height) = 0;
    unsigned int    framebuffer     =   0;
};

class RenderTexture : public FrameBufferTexture
{
public:
    RenderTexture(int _width, int _height);
    ~RenderTexture();

private:
    void CreateFrameBuffer(int _width, int _height) override;
    unsigned int    renderbuffer    =   0;
};

class DepthTexture : public FrameBufferTexture
{
public:
    DepthTexture(int _width, int _height);
    ~DepthTexture();

private:
    void CreateFrameBuffer(int _width, int _height) override;
};