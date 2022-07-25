#include "entity_factory.h"
#include "components/mesh_renderer.h"
#include "components/transform.h"
#include "assets/model.h"

Entity EntityFactory::createDirectionalLight(const char* name, glm::vec3 rot, glm::vec3 color, float intensity)
{
    return createLight(name, LightType::DIRECTIONAL, glm::vec3(0.0), rot, color, intensity, 0.0f, 0.0f);
}

Entity EntityFactory::createPointLight(const char* name, glm::vec3 pos, glm::vec3 color, float intensity)
{
    return createLight(name, LightType::POINT, pos, glm::vec3(0.0f, 0.0f, 0.0f), color, intensity, 0.0f, 0.0f);
}

Entity EntityFactory::createSpotLight(const char* name, glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff)
{
    return createLight(name, LightType::SPOT, pos, rot, color, intensity, cutoff, outerCutoff);
}

Entity EntityFactory::createMesh(glm::vec3 pos, Material *mat, std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    Mesh mesh(primitives);

    MeshRenderer meshRenderer;
    meshRenderer.mesh = &mesh;
    meshRenderer.material = mat;
    meshRenderer.initMesh();

    Entity e;
    e.addComponent(transform);
    e.addComponent(meshRenderer);

    return e;
}

Entity EntityFactory::createLight(const char* name, LightType lightType, glm::vec3 pos, glm::vec3 rot, glm::vec3 color, float intensity, float cutoff, float outerCutoff)
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

    Entity e;
    e.name(name);
    e.addComponent(transform);
    e.addComponent(light);


    return e;
}

Entity EntityFactory::createFromMesh(const char* name, glm::vec3 pos, Material *mat, Mesh* mesh)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    MeshRenderer meshRenderer;
    meshRenderer.mesh = mesh;
    meshRenderer.material = mat;
    meshRenderer.initMesh();


    Entity e;
    e.name(name);
    e.addComponent(transform);
    e.addComponent(meshRenderer);

    return e;
}

Entity EntityFactory::createFromModel(const char* name, glm::vec3 pos, Material* mat, Model* model)
{
    Transform transform;
    transform.position(pos);
    transform.updateModelMatrix();

    Entity e;
    e.name(name);
    e.addComponent(transform);

    for (auto& mesh : model->m_meshes)
    {
        e.addChild(createFromMesh(mesh.first.c_str(), pos, mat, mesh.second));
    }

    e.updateSelfAndChild();
    return e;
}
