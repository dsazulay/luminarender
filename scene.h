//
// Created by Diego Azulay on 15/04/22.
//

#ifndef INTERACTIVEGRAPHICS_SCENE_H
#define INTERACTIVEGRAPHICS_SCENE_H


#include "entity.h"

class Scene
{
public:
    void addLight(Entity entity);
    void addObject(Entity entity);
    void addSkybox(Entity entity);

    std::vector<Entity>& lights();
    std::vector<Entity>& objects();
    Entity& skybox();

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;

private:
    std::vector<Entity> m_lights;
    std::vector<Entity> m_objects;
    Entity m_skybox;
};


#endif //INTERACTIVEGRAPHICS_SCENE_H
