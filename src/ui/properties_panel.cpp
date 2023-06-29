#include "properties_panel.h"
#include "../components/transform.h"

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

    // render material

    // render light

    ImGui::End();
}
