#pragma once
#include <filesystem>
#include <string>
#include <set>
#include <map>
#include <editor/editor_resource.h>
#include <glm/glm.hpp>

class Material;

enum ETexType
{
    RED,
    RGB,
    RGBA,
    SRGB,
    SRGBA
};

class Texture2D
{
public:
    unsigned int    id;
    int             width;
    int             height;
    int             nrChannels;
    ETexType        tex_type        = ETexType::SRGBA;
    std::string     path;
    std::string     name            = "texture";
    bool            is_editor       = false;
    bool            is_valid        = false;

    EditorResource<Material*>       textureRefs;

public:
    Texture2D(std::string _path,            ETexType type = ETexType::SRGBA, bool _is_editor = false);
    Texture2D(const char* _path,            ETexType type = ETexType::SRGBA, bool _is_editor = false);
    Texture2D(std::filesystem::path _path,  ETexType type = ETexType::SRGBA, bool _is_editor = false);
    ~Texture2D();
    void DeleteTexture2D();
    bool LoadTexture2D(const char *path, ETexType type = ETexType::RGBA);
    void ResetTextureType(ETexType type);
    static std::map<std::string, Texture2D*> LoadedTextures;
};