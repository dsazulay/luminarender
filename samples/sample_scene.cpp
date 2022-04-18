//
// Created by Diego Azulay on 16/04/22.
//

#include "sample_scene.h"
#include "../entity_factory.h"
#include "../importer.h"
#include "glad/glad.h"
#include "../primitives.h"

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

    Mesh mesh;
    mesh.setPrimitives(Primitives::getCubeMapPrimitives());

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("u_view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(mesh.vao());
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

        glBindVertexArray(mesh.vao());
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
            glBindVertexArray(mesh.vao());
            glDrawElements(GL_TRIANGLES, (int)mesh.indicesSize(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    Mesh quadMesh;
    quadMesh.setPrimitives(Primitives::getQuadPrimitives());

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

    glBindVertexArray(quadMesh.vao());
    glDrawElements(GL_TRIANGLES, (int)quadMesh.indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return envCubemap;
}

SampleScene::SampleScene(Scene* scene)
{
    m_scene = scene;
    createIrradianceMaps();
    createShaders();
    createMaterials();
    addLights();
    addSceneObjects();
    addSkybox();
}

void SampleScene::createIrradianceMaps()
{
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUTTexture;
    hdrTexture = generateCubeMapTexturesFromHDR(
            Importer::loadHDRTextureFromFile("tiber_2.hdr", "Resources/Textures/skybox"),
            irradianceMap, prefilterMap, brdfLUTTexture);

    m_scene->irradianceMap = irradianceMap;
    m_scene->prefilterMap = prefilterMap;
    m_scene->brdfLUT = brdfLUTTexture;
}

void SampleScene::createShaders()
{
    Shader simpleShader("resources/shaders/unlit.vsh", "resources/shaders/unlit.fsh");
    Shader lambertShader("resources/shaders/lambert.vsh", "resources/shaders/lambert.fsh");
    Shader skyboxShader("resources/shaders/skybox_vert.glsl", "resources/shaders/skybox_frag.glsl");
    Shader pbrShader("resources/shaders/cook-torrance_vert.glsl", "resources/shaders/cook-torrance_frag.glsl");

    m_shaders["simple"] = simpleShader;
    m_shaders["lambert"] = lambertShader;
    m_shaders["skybox"] = skyboxShader;
    m_shaders["pbr"] = pbrShader;
}

void SampleScene::createMaterials()
{
    Material matRed;
    matRed.shader = &m_shaders["lambert"];
    matRed.setProperty("u_color", glm::vec4(0.2f, 0.8f, 1.0f, 1.0f));
    matRed.setTexture("u_mainTex", Importer::loadTextureFromFile("container.jpg", "resources/textures"), 0);

    Material matBlue;
    matBlue.shader = &m_shaders["simple"];
    matBlue.setProperty("u_color", glm::vec4(1.0f, 1.0, 0.0f, 1.0f));
//    matBlue.setTexture("u_mainTex", Importer::loadTextureFromFile("container.jpg", "resources/textures"), 0);
    matBlue.setTexture("u_mainTex", m_scene->brdfLUT, 0);

    Material newMat;
    newMat.shader = &m_shaders["pbr"];
    newMat.setProperty("u_albedo", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    newMat.setProperty("u_roughness", 0.5f);
    newMat.setProperty("u_metallic", 0.1f);
    newMat.setProperty("u_ao", 1.0f);

    Material matTexture;
    matTexture.shader = &m_shaders["lambert"];
    matTexture.setProperty("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    unsigned int tex = Importer::loadTextureFromFile("container2.png", "resources/textures");
    matTexture.setTexture("u_mainTex", tex, 0);

    Material backpackMat;
    backpackMat.shader = &m_shaders["lambert"];
    backpackMat.setProperty("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    unsigned int backpackTex = Importer::loadTextureFromFile("spitfire_d.png", "resources/textures/spitfire");
    backpackMat.setTexture("u_mainTex", backpackTex, 0);

    Material skyboxMat;
    skyboxMat.shader = &m_shaders["skybox"];
    skyboxMat.setTexture("u_mainTex", hdrTexture, 0);

//    std::vector<std::string> faces = { "right.jpg", "left.jpg", "top.jpg",
//                                       "bottom.jpg", "front.jpg", "back.jpg" };
//    skyboxMat.setTexture("u_mainTex", Importer::loadCubeMapFromFiles(faces, "Resources/Textures/skybox"), 0);

    m_material["red"] = matRed;
    m_material["blue"] = matBlue;
    m_material["new"] = newMat;
    m_material["tex"] = matTexture;
    m_material["skybox"] = skyboxMat;
    m_material["backpack"] = backpackMat;
}

void SampleScene::addSceneObjects()
{
    m_scene->addObject(EntityFactory::createCube(m_cubePositions[2], &m_material["red"]));
//    m_scene->addObject(EntityFactory::createCube(m_cubePositions[4], &m_material["red"]));
//    m_scene->addObject(EntityFactory::createCube(m_cubePositions[6], &m_material["tex"]));
//
//    m_scene->addObject(EntityFactory::createQuad(m_cubePositions[7], &m_material["blue"]));
//
//    m_scene->addObject(EntityFactory::createSphere(m_cubePositions[0], &m_material["new"]));
//    m_scene->addObject(EntityFactory::createSphere(m_cubePositions[9], &m_material["new"]));

    std::vector<Mesh> meshes = Importer::loadModel("resources/models/spitfire.FBX");
    std::cout << meshes.size() << std::endl;
    for (auto mesh : meshes)
    {
        Entity e = EntityFactory::createFromMesh(m_cubePositions[0], &m_material["backpack"], mesh);
        auto t = e.getComponent<Transform>();
        t->position(glm::vec3(0.0, -3.0, -5.0));
        t->scale(glm::vec3(0.05, 0.05, 0.05));
        t->eulerAngles(glm::vec3(-90, 0, 0));
        t->updateModelMatrix();
        m_scene->addObject(e);
    }
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
    m_scene->addSkybox(EntityFactory::createCubeMap(m_cubePositions[0], &m_material["skybox"]));
}


