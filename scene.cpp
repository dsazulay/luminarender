#include "scene.h"

void Scene::addLight(Entity entity)
{
    m_lights.push_back(entity);
}

void Scene::addObject(Entity entity)
{
    m_objects.emplace_back(entity);
}

void Scene::addSkybox(Entity entity)
{
    m_skybox = entity;
}

std::vector<Entity>& Scene::lights()
{
    return m_lights;
}

std::list<Entity>& Scene::objects()
{
    return m_objects;
}

Entity& Scene::skybox()
{
    return m_skybox;
}
