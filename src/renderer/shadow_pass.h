#pragma once

#include <glm/glm.hpp>

class Material;

class ShadowPass 
{
public:
    ShadowPass();
    void render();

    glm::mat4 lightSpaceMatrix{};

private:
    void updateLightMatrices();
    void renderEntity();

    Material* m_shadowMat;
};
