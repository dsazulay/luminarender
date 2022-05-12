#include "hierarchy_panel.h"
#include "imgui.h"

void HierarchyPanel::update(Scene &scene)
{
    ImGui::Begin("Scene Hierarchy");

    for (auto& entity : scene.objects())
    {
        renderEntity(entity);
    }

    ImGui::End();
}

void HierarchyPanel::renderEntity(Entity &entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (entity.getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
    }
    else
    {
        if (ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str()))
        {
            for (auto& child : entity.getChildren())
            {
                renderEntity(child);
            }
            ImGui::TreePop();
        }
    }
}
