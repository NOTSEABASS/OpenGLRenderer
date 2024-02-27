#include <attributes.h>
#include <scene_object.h>
#include <texture.h>
#include <model.h>
#include <shader.h>
#include <imgui/imgui.h>
#include <postprocess.h>

unsigned int ATR_Material::cur_id           = 10000;
unsigned int ATR_MaterialTexture::cur_id    = 20000;
unsigned int ATR_MaterialFloat::cur_id      = 30000;
unsigned int ATR_MaterialInt::cur_id        = 40000;
unsigned int ATR_MaterialColor::cur_id      = 50000;
unsigned int ATR_PostProcessNode::cur_id    = 60000;
unsigned int ATR_MeshRenderer::cur_id        = 60000;

ATR_Transform::ATR_Transform()
{
    transform = new Transform();
}



void ATR_Transform::UI_Implement()
{
    // position
    static float tmp_position[3];
    tmp_position[0] = transform->Position().r;
    tmp_position[1] = transform->Position().g;
    tmp_position[2] = transform->Position().b;
    ImGui::DragFloat3("Position", tmp_position, 0.1f);
    transform->SetPosition(tmp_position[0], tmp_position[1], tmp_position[2]);

    // rotation
    static float tmp_rotation[3];
    tmp_rotation[0] = transform->Rotation().r;
    tmp_rotation[1] = transform->Rotation().g;
    tmp_rotation[2] = transform->Rotation().b;
    ImGui::DragFloat3("Rotation", tmp_rotation, 0.1f);
    transform->SetRotation(tmp_rotation[0], tmp_rotation[1], tmp_rotation[2]);

    // scale
    static float tmp_scale[3];
    tmp_scale[0] = transform->Scale().r;
    tmp_scale[1] = transform->Scale().g;
    tmp_scale[2] = transform->Scale().b;
    ImGui::DragFloat3("Scale", tmp_scale, 0.1f);
    transform->SetScale(tmp_scale[0], tmp_scale[1], tmp_scale[2]);
}

ATR_MaterialTexture::ATR_MaterialTexture(std::string _name, Material *_material, MaterialTexture2D *_texture) : material(_material),
                                                                                                         mat_tex(_texture),
                                                                                                         slot_name(_name)
{
    id = cur_id++;
}

void ATR_MaterialTexture::UI_Implement()
{
    std::string item_name = "null";
    std::string tex_type = "null";
    if (mat_tex->texture != nullptr)
    {
        const char* types[]= {"RED", "RGB", "RGBA", "SRGB", "SRGBA"};
        item_name = (*mat_tex->texture)->name;
        tex_type = types[(*mat_tex->texture)->tex_type];
    }
    std::string material_id = std::to_string(material->id);
    std::string atrtex_id = std::to_string(id);

    ImGui::BeginChild(("child##" + material_id + atrtex_id).c_str(), ImVec2(200, 65), ImGuiChildFlags_Border);
    ImGui::Text(slot_name.c_str());
    ImGui::Text(("name:" + item_name).c_str());
    ImGui::Text(("type:" + tex_type).c_str());
    ImGui::EndChild();
    ImGui::SameLine();
    if (mat_tex->texture != nullptr)
    {
        float width = 32;
        float height = 32;
        ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
        ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right
        ImVec4 bg_col = ImVec4(0, 0, 0, 0);
        ImVec4 tint_col = ImVec4(1, 1, 1, 1);

        std::vector<std::string> tex_names;
        auto tmp = Texture2D::LoadedTextures;
        for (std::map<string, Texture2D *>::iterator it = tmp.begin(); it != tmp.end(); it++)
        {
            tex_names.push_back(it->first);
        }

        if (ImGui::ImageButton(("texture##" + material_id + atrtex_id).c_str(), (GLuint *)(*mat_tex->texture)->id, ImVec2(width, height), uv_min, uv_max, bg_col, tint_col))
        {
            ImGui::OpenPopup(("popup##" + material_id + atrtex_id).c_str());
        }

        if (ImGui::BeginPopup(("popup##" + material_id + atrtex_id).c_str()))
        {
            for (int n = 0; n < tex_names.size(); n++)
            {
                if (ImGui::Selectable((tex_names[n] + "##" + material_id + atrtex_id + std::to_string(n)).c_str()))
                {
                    material->SetTexture(mat_tex->texture, Texture2D::LoadedTextures[tex_names[n]]);
                }
                ImGui::SameLine();
                ImGui::Image((GLuint *)Texture2D::LoadedTextures[tex_names[n]]->id, ImVec2(16, 16), uv_min, uv_max);
            }
            ImGui::EndPopup();
        }
    }
    tilling[0] = mat_tex->tilling.r; tilling[1] = mat_tex->tilling.g;
    offset[0] = mat_tex->offset.r;   offset[1] = mat_tex->offset.g;
    ImGui::DragFloat2(("tilling##" + material_id + atrtex_id).c_str(), tilling, 0.1f);
    ImGui::DragFloat2(("offset##" + material_id + atrtex_id).c_str(), offset, 0.01f);
    mat_tex->tilling.r = tilling[0]; mat_tex->tilling.g = tilling[1];
    mat_tex->offset.r = offset[0];   mat_tex->offset.g = offset[1];
}

ATR_MaterialFloat::ATR_MaterialFloat(std::string _name, Material *_material, float *_value) :   slot_name(_name), 
                                                                                                material(_material), 
                                                                                                value(_value)
{
    id = cur_id++;
}

void ATR_MaterialFloat::UI_Implement()
{
    std::string item = slot_name + "##" + std::to_string(material->id) + std::to_string(id);
    ImGui::DragFloat(item.c_str(), value, drag_speed);
}

ATR_MaterialInt::ATR_MaterialInt(std::string _name, Material *_material, int *_value) :     slot_name(_name), 
                                                                                            material(_material), 
                                                                                            value(_value)
{
    id = cur_id++;
}

void ATR_MaterialInt::UI_Implement()
{
    std::string item = slot_name + "##" + std::to_string(material->id) + std::to_string(id);
    ImGui::DragInt(item.c_str(), value, drag_speed);
}

ATR_MaterialColor::ATR_MaterialColor(std::string _name, Material *_material, float *_value) :   slot_name(_name), 
                                                                                                material(_material), 
                                                                                                value(_value)
{
    id = cur_id++;
}

void ATR_MaterialColor::UI_Implement()
{
    std::string item = slot_name + "##" + std::to_string(material->id) + std::to_string(id);
    ImGui::ColorEdit3(item.c_str(), value);
}


ATR_Material::ATR_Material(Material *_material) : material(_material)
{
    for (int i = 0; i < _material->material_variables.allTextures.size(); i++)
    {
        atr_textures.push_back(new ATR_MaterialTexture(_material->material_variables.allTextures[i]->slot_name,
                                                       _material,
                                                       &_material->material_variables.allTextures[i]->variable));
    }

    for (int i = 0; i < _material->material_variables.allInt.size(); i++)
    {
        atr_ints.push_back(new ATR_MaterialInt(_material->material_variables.allInt[i]->slot_name,
                                               _material,
                                               _material->material_variables.allInt[i]->variable));
    }

    for (int i = 0; i < _material->material_variables.allFloat.size(); i++)
    {
        atr_floats.push_back(new ATR_MaterialFloat(_material->material_variables.allFloat[i]->slot_name,
                                                   _material,
                                                   _material->material_variables.allFloat[i]->variable));
    }

    for (int i = 0; i < _material->material_variables.allVec3.size(); i++)
    {
        atr_vec3s.push_back(_material->material_variables.allVec3[i]->variable);
    }

    for (int i = 0; i < _material->material_variables.allColor.size(); i++)
    {
        atr_colors.push_back(new ATR_MaterialColor(_material->material_variables.allColor[i]->slot_name,
                                                   _material,
                                                   _material->material_variables.allColor[i]->variable));
    }
    id = cur_id++;
}

void ATR_Material::UI_Implement()
{
    ImGui::Text("Material Info:");
    const char *items[] = {"Cull off", "Cull front", "Cull back"};
    std::string item = "cull face##" + std::to_string(material->id);
    ImGui::Combo(item.c_str(), &cull_current, items, IM_ARRAYSIZE(items));
    material->cullface = (E_CULL_FACE)cull_current;

    for (int i = 0; i < atr_textures.size(); i++)
    {
        atr_textures[i]->UI_Implement();
    }

    for (int i = 0; i < atr_ints.size(); i++)
    {
        atr_ints[i]->UI_Implement();
    }

    for (int i = 0; i < atr_floats.size(); i++)
    {
        atr_floats[i]->UI_Implement();
    }

    for (int i = 0; i < atr_vec3s.size(); i++)
    {
        std::string item = "vec##" + std::to_string(material->id) + std::to_string(i);
        ImGui::DragFloat3(item.c_str(), atr_vec3s[i], 0.1f);
    }

    for (int i = 0; i < atr_colors.size(); i++)
    {
        atr_colors[i]->UI_Implement();
    }
}

ATR_MeshRenderer::ATR_MeshRenderer(MeshRenderer *_meshRenderer) : meshRenderer(_meshRenderer)
{
    atr_material = new ATR_Material(_meshRenderer->material);
    id = cur_id++;
}

void ATR_MeshRenderer::UI_Implement()
{
    std::string title = "Mesh Renderer";
    std::string meshInfo = "vertices: null";
    if (meshRenderer->mesh != nullptr)
    {
        title = "Mesh Renderer##" + std::to_string(meshRenderer->mesh->VAO);
        meshInfo = "vertices: " + std::to_string(meshRenderer->mesh->vertices.size());
    }
    if (ImGui::CollapsingHeader(title.c_str(), true))
    {
        ImGui::Text(meshInfo.c_str());
        ImGui::SeparatorText("Setting");
        ImGui::Checkbox(("cast shadow##"+std::to_string(meshRenderer->mesh->VAO)).c_str(), &meshRenderer->cast_shadow);
        ImGui::SeparatorText("Material");

        const char* material_types[2] = {"Model", "PBR"};
        std::string item = "material##" + std::to_string(id);
        ImGui::Combo(item.c_str(), &cur_mat, material_types, IM_ARRAYSIZE(material_types));
        if (prev_mat != cur_mat)
        {
            meshRenderer->SetMaterial((EMaterialType)cur_mat);
            delete atr_material;
            atr_material = new ATR_Material(meshRenderer->material);
        }
        prev_mat = cur_mat;

        atr_material->UI_Implement();
    }
}

ATR_Light::ATR_Light(float *_color) : color(_color) {}

void ATR_Light::UI_Implement()
{
    ImGui::SeparatorText("Light Settings");
    ImGui::ColorEdit3("Light Color", color);
}

ATR_PostProcessManager::ATR_PostProcessManager(PostProcessManager* manager) : ppm(manager) 
{
    RefreshAllNode();
}


ATR_PostProcessManager::~ATR_PostProcessManager() {}

void ATR_PostProcessManager::RefreshAllNode()
{
    atr_pps.clear();
    for (auto atr_p : ppm->postprocess_list)
    {
        atr_pps.push_back(atr_p->atr_ppn);
    }
}

void ATR_PostProcessManager::UI_Implement()
{
    ImGui::Text("post process manager");
    for (int i = 0; i < atr_pps.size(); i++)
    {
        atr_pps[i]->UI_Implement();
        if(ImGui::Button(("Move Up##" + std::to_string(atr_pps[i]->id)).c_str(), ImVec2(60,20)))
        {
            if (i > 0)
            {
                auto tmp = atr_pps[i-1];
                atr_pps[i-1] = atr_pps[i];
                atr_pps[i] = tmp;
                ppm->MoveUpPostProcessOnIndex(i);
            }
        }
        ImGui::SameLine();
        if(ImGui::Button(("Move Down##" + std::to_string(atr_pps[i]->id)).c_str(), ImVec2(80,20)))
        {
            if (i + 1 < atr_pps.size())
            {
                auto tmp = atr_pps[i+1];
                atr_pps[i+1] = atr_pps[i];
                atr_pps[i] = tmp;
                ppm->MoveDownPostProcessOnIndex(i);
            }
        }
    }
}

ATR_PostProcessNode::ATR_PostProcessNode(PostProcess* _postprocess) : postprocess(_postprocess)
{
    id = cur_id++;
}
ATR_PostProcessNode::~ATR_PostProcessNode() {}
void ATR_PostProcessNode::UI_Implement()
{
    ImGui::SeparatorText(postprocess->name.c_str());
    ImGui::Checkbox(("enabled##" + std::to_string(id)).c_str(), &postprocess->enabled);
}

ATR_BloomProcessNode::ATR_BloomProcessNode(PostProcess* _bloomprocess) : ATR_PostProcessNode(_bloomprocess) {}
ATR_BloomProcessNode::~ATR_BloomProcessNode() {}
void ATR_BloomProcessNode::UI_Implement()
{
    ATR_PostProcessNode::UI_Implement();
    ImGui::DragFloat("threshold", &dynamic_cast<BloomProcess*>(postprocess)->threshold, 0.05f);
    ImGui::DragFloat("exposure", &dynamic_cast<BloomProcess*>(postprocess)->exposure, 0.05f);
}