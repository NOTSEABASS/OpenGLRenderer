#pragma once
#include <attributes.h>
#include <string>
#include <vector>
#include <map>

class Model;
class Shader;

class SceneObject
{
protected:
    static unsigned int cur_id;
    bool is_editor = false;

public:
    SceneObject();
    SceneObject(std::string _name, bool _is_editor = false);
    bool IsEditor() { return is_editor; }
    virtual void RenderAttribute();
    virtual ~SceneObject();

    std::string                 name;
    std::vector<SceneObject*>   children;
    ATR_Transform               *transform;
    unsigned int                id;
};

class SceneModel : public SceneObject
{
public:
    Model                           *model;
    std::vector<ATR_MeshRenderer*>  atr_meshRenderers;
    std::vector<MeshRenderer*>      meshRenderers;

public:
    SceneModel(Model *_model, bool _is_editor = false);
    SceneModel(Model *_model, std::string _name, bool _is_editor = false);
    void DrawSceneModel();
    void OnModelRemoved();
    virtual void RenderAttribute();
    virtual ~SceneModel();
};

class SceneLight : public SceneObject
{
public:
    ATR_Light *light;
    SceneLight(std::string _name, bool _is_editor);
    virtual void RenderAttribute();
    ~SceneLight();
    glm::vec3 GetLightColor();

private:
    float light_color[3] = {1, 1, 1};
};