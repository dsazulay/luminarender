#include "geometrypass.h"

#include "../camera.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/asset_library.h"

void GeometryPass::render(Scene& scene)
{
    for (auto& entity : scene.objects())
    {
        renderEntity(*entity);
    }
}

void GeometryPass::renderEntity(Entity& entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh != nullptr && mesh->mesh != nullptr && mesh->material != nullptr)
    {
        Material* origMaterial = mesh->material;   
        Material* material = m_material;
        material->shader->use();

        material->shader->setMat4("u_model", transform->modelMatrix());
        material->shader->setMat3("u_normalMatrix", glm::transpose(
            glm::inverse(glm::mat3(transform->modelMatrix()))));

        int texCount = 0;
        for (const auto &texture: origMaterial->textures)
        {
            material->shader->setInt(texture.first, texCount);
            glActiveTexture(GL_TEXTURE0 + texCount);
            glBindTexture(GL_TEXTURE_2D, texture.second);
            texCount++;
        }

        glm::vec4 color = std::any_cast<glm::vec4>(origMaterial->getUniformData()
                .find("u_color")->second);
        material->setProperty("u_color", color);
        material->setUniformData();

        glBindVertexArray(mesh->vao());
        glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    for (auto& childEntity : entity.getChildren())
    {
        renderEntity(*childEntity);
    }
}

void GeometryPass::camera(Camera* camera)
{
    m_camera = camera;
}

GeometryPass::GeometryPass()
{
    Shader* s = AssetLibrary::instance().loadShader("GBuffer", "resources/shaders/gbuffer.glsl");
    m_material = AssetLibrary::instance().createMaterial("GBuffer", s);
}
