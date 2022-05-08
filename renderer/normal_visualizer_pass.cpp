#include "normal_visualizer_pass.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"

void NormalVisualizerPass::render(Scene &scene)
{
    for (auto& entity : scene.objects())
    {
        renderNormalVectorOfEntity(entity);

        for (auto& childEntity : entity.getChildren())
        {
            renderNormalVectorOfEntity(childEntity);
        }
    }
}

void NormalVisualizerPass::renderNormalVectorOfEntity(Entity &entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh == nullptr)
        return;
    Material* material = m_material;

    material->shader->use();

    // set object uniforms (e.g. transform)
    material->shader->setMat4("u_model", transform->modelMatrix());
    material->shader->setMat3("u_normalMatrix", glm::transpose(glm::inverse(glm::mat3(m_camera->getViewMatrix() * transform->modelMatrix()))));

    material->setUniformData();

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int)mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void NormalVisualizerPass::camera(Camera *camera)
{
    m_camera = camera;
}
