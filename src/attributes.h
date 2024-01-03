#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

/*
Attributes is similar to component,
is the feature of a SceneObject and can be rendered in detail panel
*/

class MeshRenderer;
class SceneModel;
class Texture2D;
class Material;
class Shader;

class Attribute
{
public:
    virtual void UI_Implement() = 0;
};

class ATR_Transform : public Attribute
{
public:
    glm::vec3 Position;
    glm::vec3 Rotation; // Euler
    glm::vec3 Scale;

public:
    ATR_Transform();
    void UI_Implement() override;
};

class ATR_MaterialTexture : Attribute
{
public:
    ATR_MaterialTexture( std::string _name, Material *_material, Texture2D **_texture );
    void UI_Implement() override;

    unsigned int                    id;
    std::string                     slot_name = "null";
    Texture2D                       **texture;
    Material                        *material;

private:
    static unsigned int             cur_id;
};

class ATR_MaterialFloat : Attribute
{
public:
    ATR_MaterialFloat( std::string _name, Material *_material, float *_value );
    void UI_Implement() override;

    unsigned int                    id;
    std::string                     slot_name = "null";
    float                           drag_speed = 0.1;
    float                           *value;
    Material                        *material;

private:
    static unsigned int             cur_id;
};

class ATR_MaterialInt : Attribute
{
public:
    ATR_MaterialInt( std::string _name, Material *_material, int *_value );
    void UI_Implement() override;

    unsigned int                    id;
    std::string                     slot_name = "null";
    float                           drag_speed = 1;
    int                             *value;
    Material                        *material;

private:
    static unsigned int             cur_id;
};

class ATR_MaterialColor : Attribute
{
public:
    ATR_MaterialColor( std::string _name, Material *_material, float *_value );
    void UI_Implement() override;

    unsigned int                    id;
    std::string                     slot_name = "null";
    float                           drag_speed = 0.1;
    float                           *value;
    Material                        *material;

private:
    static unsigned int             cur_id;
};

class ATR_Material : public Attribute
{
public:
    ATR_Material(Material *_material);
    void UI_Implement() override;

    Material *material;
    unsigned int id;

private:
    // attributes need to draw:
    std::vector<ATR_MaterialTexture *>      atr_textures;
    std::vector<ATR_MaterialInt *>          atr_ints;
    std::vector<ATR_MaterialFloat *>        atr_floats;
    std::vector<float *>                    atr_vec3s;
    std::vector<ATR_MaterialColor *>        atr_colors;
    int                                     cull_current = 0;
    static unsigned int                     cur_id;

};

// render a mesh's all attributes
class ATR_MeshRenderer : public Attribute
{
public:
    ATR_MeshRenderer(MeshRenderer *_meshRenderer);
    void UI_Implement() override;

    MeshRenderer *meshRenderer;

private:
    ATR_Material *atr_material;
};
