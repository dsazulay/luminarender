//
// Created by Diego Azulay on 14/04/22.
//

#ifndef INTERACTIVEGRAPHICS_RENDERER_H
#define INTERACTIVEGRAPHICS_RENDERER_H

#include <vector>
#include "components/transform.h"
#include "assets/mesh.h"
#include "entity.h"
#include "uniform_buffer_object.h"
#include "camera.h"
#include "frame_buffer.h"

class Renderer {
public:
    Renderer(float viewportWidth, float viewportHeight, glm::vec3 cameraPos);
    void bindFrameBuffer();
    void updateViewportDimensions();
    void clearFrameBuffer();
    void updateTransformMatrices();
    void setupLights(std::vector<Entity>& lights);
    void render(std::list<Entity>& objects);
    void renderSkybox(Entity& skybox);
    void renderNormalVector(std::list<Entity>& objects);

    glm::mat4 cascadeShadows(glm::vec3 lightDir);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    unsigned int getTexcolorBufferID();

    Material* mat;

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;

private:
    UniformBufferObject m_matricesUBO;
    UniformBufferObject m_lightUBO;

    float m_viewportWidth;
    float m_viewportHeight;

    FrameBuffer m_viewportFrameBuffer;

    Camera m_camera;

    void onViewportResize(const Event& e);
    void renderEntity(Entity& entity);
    void renderNormalVectorOfEntity(Entity &entity);
};


#endif //INTERACTIVEGRAPHICS_RENDERER_H
