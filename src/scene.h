#pragma once

#include "entity.h"

class Scene
{
public:
    void addLight(std::unique_ptr<Entity> entity);
    void addObject(std::unique_ptr<Entity> entity, Entity* parent = nullptr);
    void addSkybox(std::unique_ptr<Entity> entity);

    std::vector<std::unique_ptr<Entity>>& lights();
    std::vector<std::unique_ptr<Entity>>& objects();
    Entity& skybox();
    Entity* mainLight() const;
    void mainLight(Entity* light);
    bool hasSkybox();
    Entity* selected();
    void selected(Entity* entity);

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;

private:
    std::vector<std::unique_ptr<Entity>> m_lights;
    std::vector<std::unique_ptr<Entity>> m_objects;
    std::unique_ptr<Entity> m_skybox;
    bool m_hasSkybox{};
    Entity* m_mainLight = nullptr;
    Entity* m_selected = nullptr;
};
