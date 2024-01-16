#pragma once
#include <string>

class RenderTexture
{
public:
    unsigned int    color_buffer;
    int             width;
    int             height;
    std::string     name;

    RenderTexture(int _width, int _height);
    void SetAsRenderTarget();
    void SetAsReadTarget();
    void BindFrameBuffer();
    ~RenderTexture();

private:
    void CreateFrameBuffer(int _width, int _height);
    unsigned int    framebuffer     =   0;
    unsigned int    renderbuffer    =   0;
};