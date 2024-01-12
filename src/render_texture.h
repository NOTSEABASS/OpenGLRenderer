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
    ~RenderTexture();

};