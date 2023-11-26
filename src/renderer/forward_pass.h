#pragma once

#include <glm/glm.hpp>

class ForwardPass
{
public:
    void render();

    glm::mat4 lightSpaceMatrix;

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;
    unsigned int shadowMap;


private:
    void renderEntity();
    void renderSkybox();
};
