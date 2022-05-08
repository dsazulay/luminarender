#include "forward_pass.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include <glad/glad.h>

void ForwardPass::render(std::list<Entity> &objects)
{
    updateViewportDimensions();
    bindFrameBuffer();
    clearFrameBuffer();

    for (auto& entity : objects)
    {
        renderEntity(entity);

        for (auto& childEntity : entity.getChildren())
        {
            renderEntity(childEntity);
        }
    }

    unbindFrameBuffer();
}

void ForwardPass::renderEntity(Entity &entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh == nullptr)
        return;
    Material* material = mesh->material;


    material->shader->use();

    // set object uniforms (e.g. transform)
    material->shader->setMat4("u_model", transform->modelMatrix());
    material->shader->setMat3("u_normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform->modelMatrix()))));

    // shadow
    auto t = m_light->getComponent<Transform>();
    material->shader->setVec3("u_lightPos", t->getDirection() * -10.0f);
    material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

    // set material uniforms (e.g. color, textures)
    int texCount = 0;
    for (const auto& texture : material->textures)
    {
        material->shader->setInt(texture.first, texCount);
        glActiveTexture(GL_TEXTURE0 + texCount);
        glBindTexture(GL_TEXTURE_2D, texture.second);
        texCount++;
    }

    // bind global illumination textures
    material->shader->setInt("u_irradianceTex", 10);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    material->shader->setInt("u_prefilterMap", 11);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

    material->shader->setInt("u_brdfLUT", 12);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, brdfLUT);

    material->setUniformData();

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int)mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

ForwardPass::ForwardPass(int width, int height, FrameBuffer::Type type) : RenderSystem(width, height, type)
{

}
