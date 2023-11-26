#include "properties_system.h"

#include "../assets/asset_library.h"
#include "../assets/mesh.h"
#include "../assets/material.h"
#include "../renderer/transform_system.h"
#include "../log.h"

#include <imgui.h>
#include "../tinyfiledialogs.h"

#include <string>

void PropertiesSystem::init(ecs::Coordinator* coordinator,
        std::optional<ecs::Entity>* selected)
{
    m_coordinator = coordinator;
    m_selected = selected;
}

void PropertiesSystem::update()
{
    ImGui::Begin("Properties");
    if (!m_selected->has_value())
    {
        ImGui::End();
        return;
    }
    
    auto& tag = m_coordinator->getComponent<ecs::Tag>(m_selected->value());
    static const int bufferSize = 256;
    static char renameBuffer[bufferSize] = "";
    strncpy(renameBuffer, tag.name.c_str(), bufferSize);
    ImGui::InputText("Name", renameBuffer, bufferSize);
    if (ImGui::IsItemDeactivated())
    {
        if (strncmp(renameBuffer, "", bufferSize) != 0)
            tag.name = renameBuffer;
    }

    auto& transform = m_coordinator->getComponent<ecs::Transform>(
            m_selected->value());
    draw(transform);
    m_coordinator->getSytem<TransformSystem>()->updateSeflAndChilren(m_selected->value());
 

    if (m_coordinator->hasComponent<ecs::MeshRenderer>(m_selected->value()))
    {
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                m_selected->value());
        draw(meshRenderer);
    }

    if (ImGui::BeginPopupContextWindow("Add Comp Popup"))
    {
        if (ImGui::Selectable("Mesh"))
        {
            m_coordinator->addComponent(m_selected->value(), ecs::MeshRenderer{
                .mesh = AssetLibrary::instance().getMesh("sphere"),
                .material = AssetLibrary::instance().getMaterial("greyMat"),
            });
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

void PropertiesSystem::draw(ecs::Transform& transform)
{
    if (ImGui::TreeNodeEx("transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
    {
        ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.1f);
        ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);

        m_coordinator->getSytem<TransformSystem>()->updateModelMatrix(transform);

        ImGui::TreePop();
    }
}

void PropertiesSystem::draw(ecs::MeshRenderer& renderer)
{
    if (ImGui::TreeNodeEx("meshRenderer", ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer"))
    {
        // TODO: add mesh renderer without mesh
        const char* items[] = { "Quad", "Cube", "Sphere" };
        // TODO: remove -1 when able to have mesh renderer without mesh
        int currentItem = ((int) renderer.mesh->meshType()) - 1;
        ImGui::Combo("Mesh Type", &currentItem, items, 3);

        if (((int) renderer.mesh->meshType() - 1) != currentItem)
        {
            switch (currentItem)
            {
            case 0:
                renderer.mesh = AssetLibrary::instance().getMesh("quad");
                renderer.mesh->meshType(MeshType::Quad);
                break;
            case 1:
                renderer.mesh = AssetLibrary::instance().getMesh("cube");
                renderer.mesh->meshType(MeshType::Cube);
                break;
            case 2:
                renderer.mesh = AssetLibrary::instance().getMesh("sphere");
                renderer.mesh->meshType(MeshType::Sphere);
                break;
            }
        }

        draw(renderer.material);
        ImGui::TreePop();
    }
}

void PropertiesSystem::draw(Material* mat)
{
    if (mat == nullptr)
        return;

    if (ImGui::TreeNodeEx("material", ImGuiTreeNodeFlags_DefaultOpen, "Material"))
    {
        ImGui::Text("%s", mat->name().c_str());
        ImGui::TreePop();
    }
}


