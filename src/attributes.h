#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <transform.h>

class MeshRenderer;
class SceneModel;
class Texture2D;
class Material;
class Shader;

/*****************************************************
* Attribute is a container to display content
* which is the feature of a SceneObject 
* and can be rendered on detail panel
*****************************************************/
class Attribute
{
public:
    virtual void UI_Implement() = 0;
    virtual ~Attribute() {};
};

class ATR_Transform : public Attribute
{
public:
    Transform* transform;

public:
    ATR_Transform();
    void UI_Implement() override;
 
    ~ATR_Transform()    override = default;
};

class ATR_MaterialTexture : Attribute
{
public:
    ATR_MaterialTexture( std::string _name, Material *_material, Texture2D **_texture );
    void UI_Implement()     override;
    ~ATR_MaterialTexture()  override = default;

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
    void UI_Implement()     override;
    ~ATR_MaterialFloat()    override = default;

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
    ~ATR_MaterialInt()  override = default;

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
    void UI_Implement()     override;
    ~ATR_MaterialColor()    override = default;

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
    void UI_Implement()     override;
    ~ATR_Material()    override = default;

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
    ~ATR_MeshRenderer() override = default;


    MeshRenderer *meshRenderer;

private:
    ATR_Material *atr_material;
};

class ATR_Light : Attribute
{
public:
    ATR_Light(float *_value);
    void UI_Implement()     override;
    ~ATR_Light()    override = default;

    float                           drag_speed = 0.1;
    float                           *color;                           
};
