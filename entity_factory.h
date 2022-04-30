#pragma once

#include "components/light.h"
#include "entity.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/model.h"

class EntityFactory {
public:
    static Entity createFromMesh(glm::vec3 pos, Material* mat, Mesh* mesh);
    static Entity createFromModel(glm::vec3 pos, Material* mat, Model* model);

    static Entity createDirectionalLight(glm::vec3 rot, glm::vec3 color, float intensity);
    static Entity createPointLight(glm::vec3 pos, glm::vec3 color, float intensity);
    static Entity createSpotLight(glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff);


private:
    static Entity createLight(LightType lightType, glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff);
    static Entity createMesh(glm::vec3 pos, Material* mat, std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitive);
};
