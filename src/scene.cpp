#include "scene.h"


void Scene::addObject(std::unique_ptr<Entity> entity, Entity* parent)
{
    if (parent != nullptr)
        parent->addChild(std::move(entity));
    else
        m_objects.push_back(std::move(entity));
}

void Scene::addSkybox(std::unique_ptr<Entity> entity)
{
    m_skybox = std::move(entity);
    m_hasSkybox = true;
}

std::vector<std::unique_ptr<Entity>>& Scene::objects()
{
    return m_objects;
}

Entity& Scene::skybox()
{
    return *m_skybox;
}

bool Scene::hasSkybox()
{
    return m_hasSkybox;
}

Entity* Scene::mainLight() const
{
    return m_mainLight;
}

void Scene::mainLight(Entity *light)
{
    m_mainLight = light;
}

void Scene::selected(Entity* entity)
{
    m_selected = entity;
}

Entity* Scene::selected()
{
    return m_selected;
}
