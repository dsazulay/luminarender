#include "panel.h"

#include "../assets/asset_library.h"
#include "../scene.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/mesh.h"
#include "../assets/material.h"
#include "../events/event.h"
#include "../events/dispatcher.h"
#include "../entity_factory.h"
#include "../log.h"

#include "../tinyfiledialogs.h"

#include <optional>
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

namespace ui::mainmenu
{
    void drawMenuFile(Scene* scene);
    void drawMenuEdit();
}

void ui::mainmenu::draw(Scene* scene)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            drawMenuFile(scene);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            drawMenuEdit(); 
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void ui::mainmenu::drawMenuFile(Scene* scene)
{
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::MenuItem("Import model"))
    {
        const char* selection = tinyfd_openFileDialog("Select File",
                "", 0, nullptr, nullptr, 0);

        if (selection != nullptr)
        {
            // TODO: extract file name + check for name repetition
            auto model = AssetLibrary::instance().loadModel(selection, selection, true);
            auto entity = EntityFactory::createFromModel("name", model);
            scene->addObject(std::move(entity));
        }
    }
}

void ui::mainmenu::drawMenuEdit()
{
    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
}
 
namespace ui::properties
{
    void draw(Material* mat);
}


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
    updateSeflAndChilren(m_selected->value());
 

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

        updateModelMatrix(transform);

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
        LOG_WARN("{}", currentItem);
        ImGui::Combo("Mesh Type", &currentItem, items, 3);

        if (((int) renderer.mesh->meshType() - 1) == currentItem)
        {
            draw(renderer.material);
            ImGui::TreePop();
            return;
        }

        switch (currentItem)
        {
            case 0:
                renderer.mesh = AssetLibrary::instance().getMesh("quad");
                renderer.mesh->meshType(MeshType::Quad);
                renderer.material = AssetLibrary::instance().getMaterial("greyMat");
                break;
            case 1:
                renderer.mesh = AssetLibrary::instance().getMesh("cube");
                renderer.mesh->meshType(MeshType::Cube);
                renderer.material = AssetLibrary::instance().getMaterial("greyMat");
                break;
            case 2:
                renderer.mesh = AssetLibrary::instance().getMesh("sphere");
                renderer.mesh->meshType(MeshType::Sphere);
                renderer.material = AssetLibrary::instance().getMaterial("greyMat");
                break;
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

void PropertiesSystem::updateModelMatrix(ecs::Transform& transform)
{
    transform.quaternion = glm::quat(glm::radians(transform.rotation));
    transform.modelMatrix = glm::mat4(1.0);
    transform.modelMatrix = glm::translate(transform.modelMatrix, transform.position);
    transform.modelMatrix = transform.modelMatrix * glm::mat4_cast(transform.quaternion);
    transform.modelMatrix = glm::scale(transform.modelMatrix, transform.scale);
}

void PropertiesSystem::updateSeflAndChilren(ecs::Entity entity) 
{
    auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
    updateModelMatrix(transform);
    if (auto parent = transform.parent)
    {
        auto& parentTransform = m_coordinator->getComponent<ecs::Transform>(*parent);
        transform.modelMatrix = parentTransform.modelMatrix * transform.modelMatrix;
    }

    for (auto child : transform.children)
    {
        updateSeflAndChilren(child);
    }
}


