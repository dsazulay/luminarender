#include "properties_panel.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"

#include "imgui.h"

char PropertiesPanel::renameBuffer[PropertiesPanel::bufferSize] = "";

void PropertiesPanel::update(Entity* entity)
{
    ImGui::Begin("Properties");
    strncpy(renameBuffer, entity->name().c_str(), bufferSize);
    ImGui::InputText("Name", renameBuffer, bufferSize);
    if (ImGui::IsItemDeactivated())
    {
        if (strncmp(renameBuffer, "", bufferSize) != 0)
        {
            entity->name(renameBuffer);
        }
    }

    auto transform = entity->getComponent<Transform>();
    if (ImGui::TreeNodeEx("transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
    {
        glm::vec3 pos = transform->position();
        ImGui::DragFloat3("Position", &pos.x, 0.1f);
        glm::vec3 rot = transform->eulerAngles();
        ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
        glm::vec3 scale = transform->scale();
        ImGui::DragFloat3("Scale", &scale.x, 0.1f);

        transform->position(pos);
        transform->eulerAngles(rot);
        transform->scale(scale);

        transform->updateModelMatrix();
        entity->updateSelfAndChild();

        ImGui::TreePop();
    }

    auto meshRenderer = entity->getComponent<MeshRenderer>();
    if (meshRenderer != nullptr)
    {
        if (ImGui::TreeNodeEx("meshRenderer", ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer"))
        {
            const char* items[] = { "Quad", "Cube", "Sphere", "Custom" };
            static int currentItem = 0; // If the selection isn't within 0..count, Combo won't display a preview
            ImGui::Combo("Mesh Type", &currentItem, items, 4);

            if (currentItem == 3)
            {
                ImGui::Text("Mesh Path");
            }

            ImGui::Text("%s", meshRenderer->material->name().c_str());
            ImGui::SameLine();
            ImGui::Text("Material");

            ImGui::TreePop();
        }
    }

    if (ImGui::BeginPopupContextWindow("Add Comp Popup"))
    {
        if (ImGui::Selectable("Mesh"))
        {
            LOG_INFO("Add mesh");
        }
        if (ImGui::Selectable("Light"))
        {
            LOG_INFO("Add light");
        }
        ImGui::EndPopup();
    }
   
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("Add Comp Popup");
    }

    ImGui::End();
}
