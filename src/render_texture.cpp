#include <glad/glad.h>
#include <render_texture.h>
#include <iostream>

RenderTexture::RenderTexture(int _width, int _height) : width(_width), height(_height)
{
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    // HDR
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);
    // LDR
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

RenderTexture::~RenderTexture()
{   
    std::cout << "Delete RenderTexture" << std::endl;
    glDeleteTextures(1, &color_buffer);
}
