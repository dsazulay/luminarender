//
// Created by Diego Azulay on 14/04/22.
//

#ifndef INTERACTIVEGRAPHICS_RENDERER_H
#define INTERACTIVEGRAPHICS_RENDERER_H

#include <vector>
#include "components/transform.h"
#include "components/mesh.h"
#include "entity.h"
#include "uniform_buffer_object.h"

class Renderer {
public:
    Renderer();
    void setupLights(std::vector<Entity>& lights);
    void render(std::vector<Entity>& objects);
    void renderSkybox(Entity& skybox);

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;

private:
    UniformBufferObject m_lightUBO;
};


#endif //INTERACTIVEGRAPHICS_RENDERER_H
