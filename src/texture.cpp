#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <material.h>
#include <texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <algorithm>
#include <string>

std::map<std::string, Texture2D *> Texture2D::LoadedTextures;

Texture2D::Texture2D(std::string _path, bool _is_editor) : path(_path), is_editor(_is_editor)
{
    is_valid = LoadTexture2D(path.c_str());
}

Texture2D::Texture2D(const char *_path, bool _is_editor) : path(_path), is_editor(_is_editor)
{
    is_valid = LoadTexture2D(path.c_str());
}

Texture2D::Texture2D(std::filesystem::path _path, bool _is_editor) : path(_path.string()), is_editor(_is_editor)
{
    is_valid = LoadTexture2D(path.c_str());
}

Texture2D::~Texture2D()
{
    DeleteTexture2D();
    LoadedTextures.erase(name);
}

void Texture2D::DeleteTexture2D()
{
    std::cout << "Delete Texture: " << path << std::endl;
    for (auto it : textureRefs.references)
    {
        it->OnTextureRemoved(this);
    }
    
    glDeleteTextures(1, &id);
}

bool Texture2D::LoadTexture2D(const char *path)
{
    int width, height, nrComponents;
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::string path_s = path;
    std::replace(path_s.begin(), path_s.end(), '\\', '/');
    unsigned char *data = stbi_load(path_s.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        this->path = path_s;
        this->width = width;
        this->height = height;
        this->nrChannels = nrComponents;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture at: " << path_s.c_str() << std::endl;
        stbi_image_free(data);
        return false;
    }
    stbi_image_free(data);
    name = path_s.substr(path_s.find_last_of('/') + 1, path_s.size());
    LoadedTextures.insert(std::map<std::string, Texture2D *>::value_type(name, this));
    return true;
}