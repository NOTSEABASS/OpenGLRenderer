#include <scene_object.h>
#include <model.h>

unsigned int SceneObject::cur_id = 0;

SceneObject::SceneObject()
{
    id = cur_id++;
    this->name = "object";
    transform = new ATR_Transform();
}

SceneObject::SceneObject(std::string _name, bool _is_editor) : name(_name), is_editor(_is_editor)
{ 
    id = cur_id++; 
    transform = new ATR_Transform();
}

SceneObject::~SceneObject() {}

void SceneObject::RenderAttribute() 
{
    transform->UI_Implement();
}

SceneModel::SceneModel(Model *_model, bool _is_editor) : model(_model)
{
    is_editor = _is_editor;
    for (int i = 0; i < _model->meshes.size(); i++)
    {
        Material* material;
        if (_model->meshes[i]->textures.size() > 0)
        {
            // material = new ModelMaterial(Shader::LoadedShaders["model.fs"], _model->meshes[i]->textures[0]);
            material = new PBRMaterial(Shader::LoadedShaders["PBR.fs"]);
            PBRMaterial* tmp = dynamic_cast<PBRMaterial*>(material);
            material->SetTexture(&tmp->albedo_map, _model->meshes[i]->textures[0]);
        }
        else
        {
            // material = new ModelMaterial(Shader::LoadedShaders["model.fs"]);
            material = new PBRMaterial(Shader::LoadedShaders["PBR.fs"]);
        }
        MeshRenderer* _meshRenderer = new MeshRenderer(material, _model->meshes[i]);
        atr_meshRenderers.push_back(new ATR_MeshRenderer(_meshRenderer));
        meshRenderers.push_back(_meshRenderer);
    }
    _model->refSceneModels.AddRef(this);
}

SceneModel::SceneModel(Model *_model, std::string _name, bool _is_editor) : SceneModel(_model, _is_editor) { name = _name; }

void SceneModel::OnModelRemoved()
{
    model = nullptr;
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        meshRenderers[i]->mesh = nullptr;
        atr_meshRenderers[i]->meshRenderer = meshRenderers[i];
    }
}

void SceneModel::RenderAttribute()
{
    transform->UI_Implement();
    for (int i = 0; i < atr_meshRenderers.size(); i++)
    {
        atr_meshRenderers[i]->UI_Implement();
    }
}

void SceneModel::DrawSceneModel()
{
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        meshRenderers[i]->Draw();
    }
}

SceneModel::~SceneModel() 
{
    if (model != nullptr)
    {
        model->refSceneModels.RemoveRef(this);
    }
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        delete atr_meshRenderers[i];
        delete meshRenderers[i];
        atr_meshRenderers[i] = nullptr;
        meshRenderers[i] = nullptr;
    }
    delete transform;
}

SceneLight::SceneLight(std::string _name, bool _is_editor) : SceneObject(_name, _is_editor) 
{
    light = new ATR_Light(light_color);
}

glm::vec3 SceneLight::GetLightColor()
{
    return glm::vec3(light_color[0], light_color[1], light_color[2]);
}

void SceneLight::RenderAttribute()
{
    transform->UI_Implement();
    light->UI_Implement();
}

SceneLight::~SceneLight() {}