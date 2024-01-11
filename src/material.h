#pragma once

#include <vector>
#include <string>
#include <texture.h>
#include <editor_content.h>

class Shader;

enum E_CULL_FACE
{
	culloff 		= 0,
	cullfront 		= 1,
	cullback 		= 2
};

template <class T>
class MaterialSlot
{
public:
	MaterialSlot(std::string _name, T _variable) : variable(_variable), slot_name(_name) {}
	
	std::string slot_name;
	T variable;
};

struct MaterialVariables
{
	std::vector<MaterialSlot<Texture2D**>*> 	allTextures;
	std::vector<MaterialSlot<int*>*> 			allInt;
	std::vector<MaterialSlot<float*>*> 			allFloat;
	std::vector<MaterialSlot<float*>*> 			allVec3;
	std::vector<MaterialSlot<float*>*> 			allColor;
};

class Material
{
public:
	std::string name;
	unsigned int id;
	Shader* shader;
	E_CULL_FACE cullface = E_CULL_FACE::culloff;
	void SetTexture(Texture2D** slot, Texture2D* new_tex);
	MaterialVariables material_variables;

private:
	static unsigned int cur_id;

public:
	Material(Shader* _shader);
    virtual ~Material();
	bool IsValid();
	virtual void Setup(std::vector<Texture2D*> default_textures) = 0;
	void OnTextureRemoved(Texture2D* removed_texture);

protected:
	void DefaultSetup(std::vector<Texture2D*> default_textures);
};

class ModelMaterial : public Material
{
public:
	// Definations of material parameters:
	float color[3] 		= { 1, 1, 1 };
	Texture2D* albedo 	= EditorContent::editor_tex["default_tex"];

public:
	ModelMaterial(Shader* _shader);
	ModelMaterial(Shader* _shader, Texture2D* albedo);
	~ModelMaterial() 										override;
	void Setup(std::vector<Texture2D*> default_textures) 	override;
};

class PBRMaterial : public Material
{
public:
	// Definations of material parameters:
	Texture2D* albedo_map 			= EditorContent::editor_tex["default_tex"];
	Texture2D* normal_map 			= EditorContent::editor_tex["default_normal_map"];
	Texture2D* ao_map 				= EditorContent::editor_tex["default_tex"];
	Texture2D* roughness_map 		= EditorContent::editor_tex["default_tex"];
	Texture2D* metal_map 			= EditorContent::editor_tex["default_tex"];
    Texture2D* spec_map 			= EditorContent::editor_tex["default_tex"];
	float color[3] 					= { 1, 1, 1 };
	float specular_color[3] 		= { 1, 1, 1 };
	float normal_strength 			= 1;
    float ao_strength 				= 1;
    float roughness_strength 		= 1;
    float metal_strength 			= 0;

public:
	PBRMaterial(Shader* _shader);
    ~PBRMaterial() 											override;
	void Setup(std::vector<Texture2D*> default_textures) 	override;
};