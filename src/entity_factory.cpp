#include "entity_factory.h"

#include "components/mesh_renderer.h"
#include "components/transform.h"
#include "assets/model.h"
#include "assets/asset_library.h"

#include <string>


int EntityFactory::entityCount = 0;

std::unique_ptr<Entity> EntityFactory::createDirectionalLight(const char* name, glm::vec3 rot, glm::vec3 color, float intensity)
{
    return createLight(name, LightType::DIRECTIONAL, glm::vec3(0.0), rot, color, intensity, 0.0f, 0.0f);
}

std::unique_ptr<Entity> EntityFactory::createPointLight(const char* name, glm::vec3 pos, glm::vec3 color, float intensity)
{
    return createLight(name, LightType::POINT, pos, glm::vec3(0.0f, 0.0f, 0.0f), color, intensity, 0.0f, 0.0f);
}

std::unique_ptr<Entity> EntityFactory::createSpotLight(const char* name, glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff)
{
    return createLight(name, LightType::SPOT, pos, rot, color, intensity, cutoff, outerCutoff);
}

std::unique_ptr<Entity> EntityFactory::createEmpty()
{
    Transform t;
    auto e = std::make_unique<Entity>();

    e->name(e->name() + std::to_string(EntityFactory::entityCount++));
    e->addComponent(t);

    return e;
}

std::unique_ptr<Entity> EntityFactory::createMesh(glm::vec3 pos, Material *mat, std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    VertexIndexTuple m;
    m.vertices = primitives.first;
    m.indices = primitives.second;
    Mesh mesh(m);

    MeshRenderer meshRenderer;
    meshRenderer.mesh = &mesh;
    meshRenderer.material = mat;

    auto e = std::make_unique<Entity>();
    e->name("New Entity");
    e->addComponent(transform);
    e->addComponent(meshRenderer);

    return e;
}

std::unique_ptr<Entity> EntityFactory::createLight(const char* name, LightType lightType, glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff)
{
    Transform transform;
    transform.position(pos);
    transform.eulerAngles(rot);
    transform.updateModelMatrix();

    Light light{};
    light.lightType = lightType;
    light.color = color;
    light.intensity = intensity;
    light.attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
    light.cutoff = cutoff;
    light.outerCutoff = outerCutoff;

    auto e = std::make_unique<Entity>();
    e->name(name);
    e->addComponent(transform);
    e->addComponent(light);


    return std::move(e);
}

std::unique_ptr<Entity> EntityFactory::createFromMesh(const char* name, glm::vec3 pos, Material *mat, Mesh* mesh)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    MeshRenderer meshRenderer;
    meshRenderer.mesh = mesh;
    meshRenderer.material = mat;

    auto e = std::make_unique<Entity>();
    e->name(name);
    e->addComponent(transform);
    e->addComponent(meshRenderer);

    return std::move(e);
}

std::unique_ptr<Entity> EntityFactory::createFromModel(const char* name, glm::vec3 pos, Material* mat, Model* model)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    auto e = std::make_unique<Entity>();
    e->name(name);
    e->addComponent(transform);

    for (auto& mesh : model->m_meshes)
    {
        auto childEntity = createFromMesh(mesh.first.c_str(), pos, mat, mesh.second);
        e->addChild(std::move(childEntity));
    }

    e->updateSelfAndChild();
    return std::move(e);
}

std::unique_ptr<Entity> EntityFactory::createFromModel(const char* name, glm::vec3 pos, Model* model)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    auto e = std::make_unique<Entity>();
    e->name(name);
    e->addComponent(transform);

    for (auto& mesh : model->m_meshes)
    {
        Material* mat = AssetLibrary::instance().getMaterial(mesh.second->modelMat().c_str());
        auto childEntity = createFromMesh(mesh.first.c_str(), pos, mat, mesh.second);
        e->addChild(std::move(childEntity));
    }

    e->updateSelfAndChild();
    return std::move(e);
}


std::unique_ptr<Entity> EntityFactory::createFromModel(const char* name, Model* model)
{
    return createFromModel(name, glm::vec3(0.0f, 0.0f, 0.0f), AssetLibrary::instance().getMaterial("defaultPbr"), model);
}
