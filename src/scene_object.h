#pragma once
#include <attributes.h>
#include <string>
#include <vector>
#include <map>

class Model;
class Shader;

class SceneObject
{
private:
    static unsigned int cur_id;

public:
    SceneObject();
    SceneObject(std::string _name);
    virtual void RenderAttribute();
    virtual ~SceneObject();

    std::string                 name;
    std::vector<SceneObject*>   children;
    unsigned int                id;
};

class SceneModel : public SceneObject
{
public:
    Model                           *model;
    ATR_Transform                   *transform;
    std::vector<ATR_MeshRenderer*>  atr_meshRenderers;
    std::vector<MeshRenderer*>      meshRenderers;

public:
    SceneModel(Model *_model);
    SceneModel(Model *_model, std::string _name);
    void DrawAllMeshRenderer();
    void OnModelRemoved();
    virtual void RenderAttribute();
    virtual ~SceneModel();
};