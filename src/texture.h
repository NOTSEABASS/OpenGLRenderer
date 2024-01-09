#pragma once
#include <filesystem>
#include <string>
#include <set>
#include <map>
#include <editor_resource.h>

class Material;

class Texture2D
{
public:
    unsigned int    id;
    int             width;
    int             height;
    int             nrChannels;
    std::string     type            = "texture_diffuse";
    std::string     path;
    std::string     name            = "texture";
    bool            is_editor       = false;
    bool            is_valid        = false;

    EditorResource<Material*>       textureRefs;

public:
    Texture2D(std::string _path, bool _is_editor = false);
    Texture2D(const char* _path, bool _is_editor = false);
    Texture2D(std::filesystem::path _path, bool _is_editor = false);
    ~Texture2D();
    void DeleteTexture2D();
    bool LoadTexture2D(const char *path);
    static std::map<std::string, Texture2D*> LoadedTextures;
};