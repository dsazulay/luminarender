#include "lightingpass.h"

#include "../camera.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/asset_library.h"

void LightingPass::render()
{
    Mesh* mesh = m_fullscreenQuad;
    Material* material = m_material;
    material->shader->use();

    material->shader->setInt("u_gposition", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gposition);

    material->shader->setInt("u_normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gnormal);

    material->shader->setInt("u_albedospec", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, galbedospec);

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void LightingPass::camera(Camera* camera)
{
    m_camera = camera;
}

LightingPass::LightingPass()
{
    Shader* s = AssetLibrary::instance().loadShader("LightingPass", "resources/shaders/lightingpass.glsl");
    m_material = AssetLibrary::instance().createMaterial("LightingPass", s);
    m_fullscreenQuad = AssetLibrary::instance().getMesh("quad");
}
