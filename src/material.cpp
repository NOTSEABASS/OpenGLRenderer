#include <material.h>
#include <texture.h>
#include <Shader.h>
#include <string>

unsigned int Material::cur_id = 0;

Material::Material(Shader *_shader) : shader(_shader)                   { id = cur_id++;                                        }
Material::~Material()                                                   { std::cout << "delete Material" << std::endl;          }
ModelMaterial::~ModelMaterial()                                         { std::cout << "delete Model Material" << std::endl;    }
PBRMaterial::~PBRMaterial()                                             { std::cout << "delete PBR Material" << std::endl;      }
void ModelMaterial::Setup(std::vector<Texture2D *> default_textures)    { DefaultSetup(default_textures);                       }
void PBRMaterial::Setup(std::vector<Texture2D *> default_textures)      { DefaultSetup(default_textures);                       }
bool Material::IsValid()                                                { return shader != nullptr;                             }

void Material::SetTexture(Texture2D **slot, Texture2D *new_tex)
{
    (*slot)->textureRefs.RemoveRef(this);
    (*slot) = new_tex;
    (*slot)->textureRefs.AddRef(this);
}

void Material::DefaultSetup(std::vector<Texture2D *> default_textures)
{
    unsigned int tex_id = 0;
    for (auto tex : material_variables.allTextures)
    {
        glActiveTexture(GL_TEXTURE0 + tex_id);
        glUniform1i(glGetUniformLocation(shader->ID, (tex->slot_name).c_str()), tex_id);
        glBindTexture(GL_TEXTURE_2D, (*tex->variable)->id);
        tex_id++;
    }
    shader->use();
    for (auto value : material_variables.allColor)
    {
        shader->setVec3(value->slot_name.c_str(), glm::vec3(value->variable[0], value->variable[1], value->variable[2]));
    }
    for (auto value : material_variables.allFloat)
    {
        shader->setFloat(value->slot_name.c_str(), *value->variable);
    }
    for (auto value : material_variables.allInt)
    {
        shader->setInt(value->slot_name.c_str(), *value->variable);
    }
    for (auto value : material_variables.allVec3)
    {
        shader->setVec3(value->slot_name.c_str(), glm::vec3(value->variable[0], value->variable[1], value->variable[2]));
    }
}

void Material::OnTextureRemoved(Texture2D *removed_texture)
{
    for (int i = 0; i < material_variables.allTextures.size(); i++)
    {
        if (*(material_variables.allTextures[i]->variable) == removed_texture)
        {
            this->SetTexture(material_variables.allTextures[i]->variable, EditorContent::editor_tex["default_tex"]);
        }
    }
}

ModelMaterial::ModelMaterial(Shader *_shader) : Material::Material(_shader)
{
    albedo = EditorContent::editor_tex["default_tex"];
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("albedo", &albedo));
    albedo->textureRefs.AddRef(this);
    material_variables.allColor.push_back(new MaterialSlot<float *>("color", color));
}

ModelMaterial::ModelMaterial(Shader *_shader, Texture2D *_albedo) : Material::Material(_shader)
{
    albedo = _albedo;
    albedo->textureRefs.AddRef(this);
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("albedo_map", &albedo));
    material_variables.allColor.push_back(new MaterialSlot<float *>("color", color));
}

PBRMaterial::PBRMaterial(Shader *_shader) : Material::Material(_shader)
{
    // Init all material variables
    albedo_map->textureRefs.AddRef(this);
    normal_map->textureRefs.AddRef(this);
    ao_map->textureRefs.AddRef(this);
    roughness_map->textureRefs.AddRef(this);
    metal_map->textureRefs.AddRef(this);
    spec_map->textureRefs.AddRef(this);

    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("albedo_map", &albedo_map));
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("normal_map", &normal_map));
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("ao_map", &ao_map));
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("roughness_map", &roughness_map));
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("metal_map", &metal_map));
    material_variables.allTextures.push_back(new MaterialSlot<Texture2D **>("spec_map", &spec_map));

    material_variables.allColor.push_back(new MaterialSlot<float *>("color", color));
    material_variables.allColor.push_back(new MaterialSlot<float *>("specularColor", specular_color));
    material_variables.allFloat.push_back(new MaterialSlot<float *>("normalStrength", &normal_strength));
    material_variables.allFloat.push_back(new MaterialSlot<float *>("aoStrength", &ao_strength));
    material_variables.allFloat.push_back(new MaterialSlot<float *>("roughnessStrength", &roughness_strength));
    material_variables.allFloat.push_back(new MaterialSlot<float *>("metalStrength", &metal_strength));
}
