#pragma once

#include "entity.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/model.h"

class EntityFactory {
public:
    static std::unique_ptr<Entity> createEmpty();
    static std::unique_ptr<Entity> createFromMesh(const char* name, glm::vec3 pos, Material* mat, Mesh* mesh);
    static std::unique_ptr<Entity> createFromModel(const char* name, glm::vec3 pos, Material* mat, Model* model);
    static std::unique_ptr<Entity> createFromModel(const char* name, glm::vec3 pos, Model* model);
    static std::unique_ptr<Entity> createFromModel(const char* name, Model* model);

private:
    static std::unique_ptr<Entity> createMesh(glm::vec3 pos, Material* mat, std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitive);
    static int entityCount;
};
