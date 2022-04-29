//
// Created by Diego Azulay on 16/04/22.
//

#include "sample_scene.h"
#include "../entity_factory.h"
#include "glad/glad.h"
#include "../primitives.h"
#include "../components/mesh_renderer.h"
#include "../log.h"

unsigned int generateCubeMapTexturesFromHDR(unsigned int hdrTexture, unsigned int &irradianceMap, unsigned int &prefilterMap, unsigned int &brdfLUTTexture)
{
    Shader equirectangularToCubemapShader("resources/shaders/skybox_equirectangular_vert.glsl", "resources/shaders/skybox_equirectangular_frag.glsl");
    Shader irradianceShader("resources/shaders/skybox_equirectangular_vert.glsl", "resources/shaders/skybox_irradiance_frag.glsl");
    Shader prefilterShader("resources/shaders/skybox_equirectangular_vert.glsl", "resources/shaders/prefilter_frag.glsl");
    Shader brdfShader("resources/shaders/brdf_vert.glsl", "resources/shaders/brdf_frag.glsl");

    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
            {
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
            };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("u_equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("u_projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    Mesh mesh(Primitives::getCubeMapPrimitives());
    MeshRenderer mr;
    mr.mesh = &mesh;
    mr.initMesh();

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("u_view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(mr.vao());
        glDrawElements(GL_TRIANGLES, (int)mesh.indicesSize(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    irradianceShader.use();
    irradianceShader.setInt("u_environmentMap", 0);
    irradianceShader.setMat4("u_projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("u_view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(mr.vao());
        glDrawElements(GL_TRIANGLES, (int)mesh.indicesSize(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.use();
    prefilterShader.setInt("u_environmentMap", 0);
    prefilterShader.setMat4("u_projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("u_roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("u_view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(mr.vao());
            glDrawElements(GL_TRIANGLES, (int)mesh.indicesSize(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    Mesh quadMesh(Primitives::getQuadPrimitives());
    MeshRenderer quadRenderer;
    quadRenderer.mesh = &quadMesh;
    quadRenderer.initMesh();

    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(quadRenderer.vao());
    glDrawElements(GL_TRIANGLES, (int)quadMesh.indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return envCubemap;
}

SampleScene::SampleScene(Scene* scene, AssetLibrary* assetLibrary) : m_assetLibrary(assetLibrary)
{
    m_scene = scene;
    createIrradianceMaps();
    createShaders();
    createMaterials();
    loadMeshes();
    addLights();
    addSceneObjects();
    addSkybox();
}

void SampleScene::createIrradianceMaps()
{
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUTTexture;

    Texture* texture = m_assetLibrary->loadHDRTexture("tiber", "tiber_2.hdr", "Resources/Textures/skybox");
    hdrTexture = generateCubeMapTexturesFromHDR(texture->ID(),
            irradianceMap, prefilterMap, brdfLUTTexture);

    m_scene->irradianceMap = irradianceMap;
    m_scene->prefilterMap = prefilterMap;
    m_scene->brdfLUT = brdfLUTTexture;
}

void SampleScene::createShaders()
{
    m_assetLibrary->loadShader("simple", "resources/shaders/unlit.vsh", "resources/shaders/unlit.fsh");
    m_assetLibrary->loadShader("lambert", "resources/shaders/lambert.vsh", "resources/shaders/lambert.fsh");
    m_assetLibrary->loadShader("skybox", "resources/shaders/skybox_vert.glsl", "resources/shaders/skybox_frag.glsl");
    m_assetLibrary->loadShader("pbr", "resources/shaders/cook-torrance_vert.glsl", "resources/shaders/cook-torrance_frag.glsl");
}

void SampleScene::createMaterials()
{
    m_assetLibrary->load2DTexture("container", "container.jpg", "resources/textures");
    Material* matRed = m_assetLibrary->createMaterial("red", m_assetLibrary->getShader("lambert"));
    matRed->setProperty("u_color", glm::vec4(0.2f, 0.8f, 1.0f, 1.0f));
    matRed->setTexture("u_mainTex", m_assetLibrary->getTexture("container")->ID(), 0);

    Material* matBlue = m_assetLibrary->createMaterial("blue", m_assetLibrary->getShader("simple"));
    matBlue->setProperty("u_color", glm::vec4(1.0f, 1.0, 0.0f, 1.0f));
//    matBlue.setTexture("u_mainTex", Importer::loadTextureFromFile("container.jpg", "resources/textures"), 0);
    matBlue->setTexture("u_mainTex", m_scene->brdfLUT, 0);

    Material* newMat = m_assetLibrary->createMaterial("new", "pbr");
    newMat->setProperty("u_albedo", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    newMat->setProperty("u_roughness", 0.5f);
    newMat->setProperty("u_metallic", 0.1f);
    newMat->setProperty("u_ao", 1.0f);

    Material* matTexture = m_assetLibrary->createMaterial("tex", "lambert");
    matTexture->setProperty("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Texture* container2 = m_assetLibrary->load2DTexture("container2", "container2.png", "resources/textures");
    matTexture->setTexture("u_mainTex", container2->ID(), 0);

    Material* backpackMat = m_assetLibrary->createMaterial("backpack", m_assetLibrary->getShader("lambert"));
    backpackMat->setProperty("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Texture* spitfire = m_assetLibrary->load2DTexture("spitfire", "spitfire_d.png", "resources/textures/spitfire");
    backpackMat->setTexture("u_mainTex", spitfire->ID(), 0);

    Material* skyboxMat = m_assetLibrary->createMaterial(
            "skybox", m_assetLibrary->getShader("skybox"));
    skyboxMat->setTexture("u_mainTex", hdrTexture, 0);

//    std::vector<std::string> faces = { "right.jpg", "left.jpg", "top.jpg",
//                                       "bottom.jpg", "front.jpg", "back.jpg" };
//    Texture* skyboxTex = m_assetLibrary.loadCubeMapTexture("skyboxTex", faces, "Resources/Textures/skybox");
//    skyboxMat->setTexture("u_mainTex", skyboxTex->ID(), 0);

}

void SampleScene::loadMeshes()
{
    m_assetLibrary->loadMesh(AssetLibrary::BasicMesh::Cube);
    m_assetLibrary->loadMesh(AssetLibrary::BasicMesh::CubeMapModel);
    m_assetLibrary->loadMesh(AssetLibrary::BasicMesh::Quad);
    m_assetLibrary->loadMesh(AssetLibrary::BasicMesh::Sphere);

    m_assetLibrary->loadModel("airplane", "resources/models/spitfire.FBX");
}

void SampleScene::addSceneObjects()
{
    Mesh* cube = m_assetLibrary->getMesh("cube");
    Mesh* quad = m_assetLibrary->getMesh("quad");
    Mesh* sphere = m_assetLibrary->getMesh("sphere");
    Entity cubeEntity = EntityFactory::createFromMesh(
            m_cubePositions[2], m_assetLibrary->getMaterial("red"),cube);
//    m_scene->addObject(EntityFactory::createFromMesh(
//            m_cubePositions[4], m_assetLibrary->getMaterial("red"), cube));
    m_scene->addObject(EntityFactory::createFromMesh(
            m_cubePositions[6], m_assetLibrary->getMaterial("tex"), cube));

    auto& entity = m_scene->objects().front();
    entity.addChild(cubeEntity);
    auto transform = entity.getComponent<Transform>();
    transform->eulerAngles(glm::vec3(0.0, 0.0, 35.0));
    transform->scale(glm::vec3(2, 2, 2));
    transform->updateModelMatrix();
    entity.updateSelfAndChild();
//
//    m_scene->addObject(EntityFactory::createFromMesh(
//            m_cubePositions[7], m_assetLibrary.getMaterial("blue"), quad));
//
//    m_scene->addObject(EntityFactory::createFromMesh(
//            m_cubePositions[0], m_assetLibrary.getMaterial("new"), sphere));
//    m_scene->addObject(EntityFactory::createFromMesh(
//            m_cubePositions[9], m_assetLibrary.getMaterial("new"), sphere));

    Entity e = EntityFactory::createFromModel(
            m_cubePositions[0], m_assetLibrary->getMaterial("backpack"),
            m_assetLibrary->getModel("airplane"));

    auto t = e.getComponent<Transform>();
    t->position(glm::vec3(0.0, -3.0, -5.0));
    t->scale(glm::vec3(0.05, 0.05, 0.05));
    t->eulerAngles(glm::vec3(-90, 0, 0));
    t->updateModelMatrix();
    e.updateSelfAndChild();

    m_scene->addObject(e);
}

void SampleScene::addLights()
{
    m_scene->addLight(EntityFactory::createDirectionalLight(glm::vec3(0.0f, -45.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.5f), 1.0f));
    m_scene->addLight(EntityFactory::createDirectionalLight(glm::vec3(-45.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f), 1.0f));

    m_scene->addLight(EntityFactory::createPointLight(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.8f, 0.5f), 1.0f));

    m_scene->addLight(EntityFactory::createSpotLight(glm::vec3(-1.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.5f, 0.2f), 1.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f))));

}

void SampleScene::addSkybox()
{
    Mesh* cubeMap = m_assetLibrary->getMesh("cubeMap");
    m_scene->addSkybox(EntityFactory::createFromMesh(
            m_cubePositions[0], m_assetLibrary->getMaterial("skybox"), cubeMap));
}


