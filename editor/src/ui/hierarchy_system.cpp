#include "hierarchy_system.h"

#include "assets/asset_library.h"
#include "assets/mesh.h"
#include "assets/material.h"
#include "renderer/transform_system.h"
#include "log.h"

#include "tinyfiledialogs.h"

#include <string>

std::string HierarchySystem::rename = "";
char HierarchySystem::renameBuffer[HierarchySystem::bufferSize] = "";

void HierarchySystem::init(ecs::Coordinator* coordinator, 
        std::optional<ecs::Entity>* selected)
{
    m_coordinator = coordinator;
    m_selected = selected;
}

void HierarchySystem::update()
{
    ImGui::Begin("Hierarchy");

    if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) 
            && ImGui::IsWindowHovered())
    {
        m_selected->reset();
    }

    if (ImGui::BeginPopupContextWindow("Scene Hierarchy Popup"))
    {
        if (ImGui::Selectable("Create Empty"))
        {
            //UiCreateEmptyEvent e{scene->selected()};
            //Dispatcher::instance().post(e);
            auto newEntity = m_coordinator->createEntity();
            m_coordinator->addComponent(newEntity, ecs::Tag {
                .name = "New Object",
            });
            m_coordinator->addComponent(newEntity, ecs::Transform {
                    .parent = *m_selected,
            });

            if (m_selected->has_value())
            {
                auto& parent = m_coordinator->getComponent<ecs::Transform>(
                        m_selected->value());
                parent.children.push_back(newEntity);
            }
        }
        ImGui::EndPopup();
    }

    updateRootEntities();
    for (auto entity : m_rootEntities)
    {
        draw(entity);
    }

    ImGui::End();
}

void HierarchySystem::draw(ecs::Entity entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow 
        | ImGuiTreeNodeFlags_SpanAvailWidth;
    
    auto& tag = m_coordinator->getComponent<ecs::Tag>(entity);
    auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);

    if (m_selected->has_value() && entity == m_selected->value())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (tag.name == rename)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::SetKeyboardFocusHere(0);
        ImGui::InputText("##rename", renameBuffer, bufferSize);
        if (ImGui::IsItemDeactivated())
        {
            if (strncmp(renameBuffer, "", bufferSize) != 0)
            {
                tag.name = renameBuffer;
            }
            renameBuffer[0] = '\0';
            rename = "";
        }

        ImGui::SetCursorPos(p);
    }

    if (transform.children.size() == 0)
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        drawTreeNode(entity, tag.name, flags);

        return;
    }

    if (drawTreeNode(entity, tag.name, flags))
    {
        for (auto child : transform.children)
        {
            draw(child);
        }
        ImGui::TreePop();
    }
}

bool HierarchySystem::drawTreeNode(ecs::Entity entity, 
        std::string& name, ImGuiTreeNodeFlags flags)
{
    bool shouldExpand = ImGui::TreeNodeEx(name.c_str(), flags, "%s", name.c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        rename = name;
    }
    if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
    {
        *m_selected = entity;
    }

    return shouldExpand;
}

void HierarchySystem::updateRootEntities()
{
    m_rootEntities.clear();

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        if (auto parent = transform.parent)
            continue;    

        m_rootEntities.push_back(entity);
    }
}

