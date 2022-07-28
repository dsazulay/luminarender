#include "hierarchy_panel.h"
#include "imgui.h"

void HierarchyPanel::update(Scene &scene)
{
    ImGui::Begin("Scene Hierarchy");

    for (auto& entity : scene.objects())
    {
        renderEntity(*entity, scene);
    }

    for (auto& entity : scene.lights())
    {
        renderEntity(*entity, scene);
    }

    ImGui::End();
}

void HierarchyPanel::renderEntity(Entity &entity, Scene& scene)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    // TODO: change this to use entity's ID instead of name
    if (scene.selected() != nullptr && entity.name() == scene.selected()->name())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (entity.getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
        if (ImGui::IsItemClicked())
        {
            scene.selected(&entity);
        }
    }
    else
    {
        bool shouldExpand = ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
        if (ImGui::IsItemClicked())
        {
            scene.selected(&entity);
        }

        if (shouldExpand)
        {
            for (auto& child : entity.getChildren())
            {
                renderEntity(*child, scene);
            }
            ImGui::TreePop();
        }
    }
}
