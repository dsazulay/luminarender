#include "scene.h"

void Scene::addLight(std::unique_ptr<Entity> entity)
{
    m_lights.push_back(std::move(entity));
}

void Scene::addObject(std::unique_ptr<Entity> entity)
{
    m_objects.push_back(std::move(entity));
}

void Scene::addSkybox(std::unique_ptr<Entity> entity)
{
    m_skybox = std::move(entity);
    m_hasSkybox = true;
}

std::vector<std::unique_ptr<Entity>>& Scene::lights()
{
    return m_lights;
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
