#include "pbr_scene.h"

#include "locator.h"
#include "sample_resources.h"
#include "irradiance_map_factory.h"
#include "components/components.h"
#include "renderer/transform_system.h"

constexpr std::string_view SPONZA_PATH = "resources/sponza_glTF/Sponza.gltf";

void PbrScene::loadScene()
{
    m_ecsCoordinator = Locator::getEcsCoordinator();
    m_assetManager = Locator::getAssetManager();

    unsigned int skyboxTex;

    loadIrradianceTextures(skyboxTex);
    loadMaterials(skyboxTex);
    loadLights();
    loadSkybox();
    loadObjects();
}


void PbrScene::loadIrradianceTextures(unsigned int& skyboxTex)
{

    IrradianceMaps maps = IrradianceMapFactory::generateIrradianceMapsFromHDR(m_assetManager->getTextureHDR("resources/textures/skybox/tiber_2.hdr").handle, *m_assetManager);

    skyboxTex = maps.cubeMap;
}

void PbrScene::loadMaterials(unsigned int& skyboxTex)
{
    Shader* pbr = &m_assetManager->getShader("resources/shaders/cook_torrance.glsl");
    Material& blueMat = m_assetManager->createMaterial("blueMat", pbr);
    blueMat.setColor("u_albedo", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));
    blueMat.setFloat("u_roughness", 0.9f);


    Material& greyMat = m_assetManager->createMaterial("greyMat", pbr);
    greyMat.setColor("u_albedo", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    greyMat.setFloat("u_roughness", 0.2f);

    Material& spitfireMat = m_assetManager->createMaterial("spitfireMat", pbr);
    spitfireMat.setTexture("u_albedoTex", m_assetManager->getTexture2D("resources/textures/spitfire/spitfire_d.png").handle);
    spitfireMat.setTexture("u_metallicTex", m_assetManager->getTexture2D("resources/textures/spitfire/spitfire_m.png").handle);
    spitfireMat.setTexture("u_roughnessTex", m_assetManager->getTexture2D("resources/textures/spitfire/spitfire_r.png").handle);
    spitfireMat.setTexture("u_aoTex", m_assetManager->getTexture2D("resources/textures/spitfire/spitfire_ao.png").handle);

    Material& metalBoxMat = m_assetManager->createMaterial("metalBoxMat", pbr);
    metalBoxMat.setTexture("u_albedoTex", m_assetManager->getTexture2D("resources/textures/metalbox/metalbox_a.png").handle);
    metalBoxMat.setTexture("u_roughnessTex", m_assetManager->getTexture2D("resources/textures/metalbox/metalbox_r.png").handle);
    metalBoxMat.setTexture("u_aoTex", m_assetManager->getTexture2D("resources/textures/metalbox/metalbox_ao.png").handle);

    Material& cerberusMat = m_assetManager->createMaterial("cerberusMat", pbr);
    cerberusMat.setTexture("u_albedoTex", m_assetManager->getTexture2D("resources/textures/cerberus/cerberus_a.png").handle);
    cerberusMat.setTexture("u_metallicTex", m_assetManager->getTexture2D("resources/textures/cerberus/cerberus_r.png").handle);
    cerberusMat.setTexture("u_roughnessTex", m_assetManager->getTexture2D("resources/textures/cerberus/cerberus_r.png").handle);

    Shader* skytri = &m_assetManager->getShader("resources/shaders/skytriangle.glsl");
    Material& skyboxMat = m_assetManager->createMaterial("skyboxMat", skytri);
    skyboxMat.setTexture("u_mainTex", skyboxTex);
}

void PbrScene::loadLights()
{
    ecs::Entity light = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(light, ecs::Light{
        .color = glm::vec3(0.9, 0.9, 0.8),
    });
    m_ecsCoordinator->addComponent(light, ecs::Transform{
        .rotation = glm::vec3(-60.0, 20.0, 0.0),
        .quaternion =  glm::quat(glm::radians(glm::vec3(-60.0, 20.0, 0.0))),
    });
    m_ecsCoordinator->addComponent(light, ecs::Tag{
        .name = "Directional Light"
    });
}

void PbrScene::loadSkybox()
{
    Mesh cubeMap = m_assetManager->getMesh(MeshType::TriangleMap);
    Material& skyboxMat = m_assetManager->getMaterial("skyboxMat");

}

void PbrScene::loadObjects()
{

    Mesh cube = m_assetManager->getMesh(MeshType::Cube);
    Mesh quad = m_assetManager->getMesh(MeshType::Quad);
    Mesh sphere = m_assetManager->getMesh(MeshType::Sphere);
    const Model& spitfire = m_assetManager->getModel(SampleResources::model_spitfire);
    const Model& cerberus = m_assetManager->getModel(SampleResources::model_cerberus);
    const Model& sponza = m_assetManager->getModel(SPONZA_PATH, true);

    Material& greyMat = m_assetManager->getMaterial("greyMat");
    Material& blueMat = m_assetManager->getMaterial("blueMat");
    Material& spitfireMat = m_assetManager->getMaterial("spitfireMat");
    Material& woodBoxMat = m_assetManager->getMaterial("metalBoxMat");
    Material& cerberusMat = m_assetManager->getMaterial("cerberusMat");

    auto sphereEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(sphereEntity, ecs::MeshRenderer{
        .mesh = sphere,
        .material = &blueMat,
    });
    m_ecsCoordinator->addComponent(sphereEntity, ecs::Transform{
        .position = SampleResources::object_positions[1],
    });
    m_ecsCoordinator->addComponent(sphereEntity, ecs::Tag{
        .name = "Sphere",
    });
 
    auto cubeEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(cubeEntity, ecs::MeshRenderer{
        .mesh = cube,
        .material = &woodBoxMat,
    });
    m_ecsCoordinator->addComponent(cubeEntity, ecs::Transform{
        .position = SampleResources::object_positions[3],
    });
    m_ecsCoordinator->addComponent(cubeEntity, ecs::Tag{
        .name = "Cube",
    });

    auto quadEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(quadEntity, ecs::MeshRenderer{
        .mesh = quad,
        .material = &greyMat,
    });
    m_ecsCoordinator->addComponent(quadEntity, ecs::Transform{
        .position = SampleResources::object_positions[0],
        .rotation = glm::vec3(-90.0, 0.0, 0.0),
        .scale = glm::vec3(10.0, 10.0, 10.0),
    });
    m_ecsCoordinator->addComponent(quadEntity, ecs::Tag{
        .name = "Quad",
    });

    auto spitfireEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(spitfireEntity, ecs::Transform{});
    m_ecsCoordinator->addComponent(spitfireEntity, ecs::Tag{
        .name = "Spitfire",
    });

    auto transformSystem = m_ecsCoordinator->getSystem<TransformSystem>();

    for (auto& mesh : spitfire.meshes)
    {
        auto newEntity = m_ecsCoordinator->createEntity();
        m_ecsCoordinator->addComponent(newEntity, ecs::Transform{});
        m_ecsCoordinator->addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = &spitfireMat,
        });
        m_ecsCoordinator->addComponent(newEntity, ecs::Tag{
            .name = mesh.first,
        });

        transformSystem->addChild(spitfireEntity, newEntity);
    }

    auto& spitfireTransform = m_ecsCoordinator->getComponent<ecs::Transform>(
            spitfireEntity);
    spitfireTransform.position = glm::vec3( 2.4f, -6.5f, -3.5f);
    spitfireTransform.rotation = glm::vec3(-96.0, 0.0, 0.0);
    spitfireTransform.scale = glm::vec3(0.05, 0.05, 0.05);


    auto cerberusEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(cerberusEntity, ecs::Transform{});
    m_ecsCoordinator->addComponent(cerberusEntity, ecs::Tag{
        .name = "Cerberus",
    });

    for (auto& mesh : cerberus.meshes)
    {
        auto newEntity = m_ecsCoordinator->createEntity();
        m_ecsCoordinator->addComponent(newEntity, ecs::Transform{});
        m_ecsCoordinator->addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = &cerberusMat,
        });
        m_ecsCoordinator->addComponent(newEntity, ecs::Tag{
            .name = mesh.first,
        });
        transformSystem->addChild(cerberusEntity, newEntity);
    }

    auto& cerberusTransform = m_ecsCoordinator->getComponent<ecs::Transform>(
            cerberusEntity);
    cerberusTransform.position = SampleResources::object_positions[5];
    cerberusTransform.rotation = glm::vec3(-96.0, 0.0, 0.0);
    cerberusTransform.scale = glm::vec3(0.05);

    auto sponzaEntity = m_ecsCoordinator->createEntity();
    m_ecsCoordinator->addComponent(sponzaEntity, ecs::Transform{});
    m_ecsCoordinator->addComponent(sponzaEntity, ecs::Tag{
        .name = "Sponza",
    });

    for (auto& mesh : sponza.meshes)
    {
        auto newEntity = m_ecsCoordinator->createEntity();
        m_ecsCoordinator->addComponent(newEntity, ecs::Transform{});

        Material* mat = &m_assetManager->getMaterial(mesh.second.importedMatName);
        m_ecsCoordinator->addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = mat,
        });
        m_ecsCoordinator->addComponent(newEntity, ecs::Tag{
            .name = mesh.first,
        });
        transformSystem->addChild(sponzaEntity, newEntity);
    }

    auto& sponzaTransform = m_ecsCoordinator->getComponent<ecs::Transform>(sponzaEntity);
    sponzaTransform.scale = glm::vec3(0.01, 0.01, 0.01);

    transformSystem->update();
    transformSystem->addChild(cubeEntity, sphereEntity);
    transformSystem->addChild(quadEntity, cubeEntity);
    transformSystem->updateHierarchically();
}
