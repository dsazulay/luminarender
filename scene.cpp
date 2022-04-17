//
// Created by Diego Azulay on 15/04/22.
//

#include "scene.h"
#include "components/transform.h"

void Scene::addLight(Entity entity)
{
    m_lights.push_back(entity);
}

void Scene::addObject(Entity entity)
{
    m_objects.push_back(entity);
}

void Scene::addSkybox(Entity entity)
{
    m_skybox = entity;
}

std::vector<Entity>& Scene::lights()
{
    return m_lights;
}

std::vector<Entity>& Scene::objects()
{
    return m_objects;
}

Entity& Scene::skybox()
{
    return m_skybox;
}
