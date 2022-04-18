//
// Created by Diego Azulay on 15/04/22.
//

#ifndef INTERACTIVEGRAPHICS_ENTITY_FACTORY_H
#define INTERACTIVEGRAPHICS_ENTITY_FACTORY_H


#include "components/transform.h"
#include "components/light.h"
#include "entity.h"
#include "material.h"
#include "components/mesh.h"

class EntityFactory {
public:
    static Entity createCube(glm::vec3 pos, Material* mat);
    static Entity createCubeMap(glm::vec3 pos, Material* mat);
    static Entity createQuad(glm::vec3 pos, Material* mat);
    static Entity createSphere(glm::vec3 pos, Material* mat);

    static Entity createFromMesh(glm::vec3 pos, Material* mat, Mesh& mesh);

    static Entity createDirectionalLight(glm::vec3 rot, glm::vec3 color, float intensity);
    static Entity createPointLight(glm::vec3 pos, glm::vec3 color, float intensity);
    static Entity createSpotLight(glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff);


private:
    static Entity createLight(LightType lightType, glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff);
    static Entity createMesh(glm::vec3 pos, Material* mat, std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitive);
};


#endif //INTERACTIVEGRAPHICS_ENTITY_FACTORY_H
