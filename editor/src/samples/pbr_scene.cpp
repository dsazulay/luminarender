#include "pbr_scene.h"

#include "assets/model.h"
#include "log.h"
#include "sample_resources.h"
#include "irradiance_map_factory.h"
#include "components/components.h"
#include "renderer/transform_system.h"


void PbrScene::loadScene(AssetLibrary &assetLibrary,
                         AssetManager& assetManager,
                         ecs::Coordinator& coordinator)
{
    unsigned int skyboxTex;

    loadTextures(assetLibrary);
    loadIrradianceTextures(assetLibrary, skyboxTex, assetManager);
    loadMaterials(assetLibrary, assetManager, skyboxTex);
    loadModels(assetLibrary);
    loadLights(coordinator);
    loadSkybox(assetLibrary, assetManager);
    loadObjects(assetLibrary, assetManager, coordinator);
}

void PbrScene::loadTextures(AssetLibrary& assetLibrary)
{
}

void PbrScene::loadIrradianceTextures(AssetLibrary& assetLibrary,
                                      unsigned int& skyboxTex,
                                      AssetManager& assetManager)
{

    IrradianceMaps maps = IrradianceMapFactory::generateIrradianceMapsFromHDR(assetManager.getTextureHDR("resources/textures/skybox/tiber_2.hdr").handle, assetManager);

    skyboxTex = maps.cubeMap;
}

void PbrScene::loadMaterials(AssetLibrary &assetLibrary, AssetManager &assetManager, unsigned int& skyboxTex)
{
    Material* blueMat = assetLibrary.createMaterial("blueMat", "pbr");
    blueMat->setProperty("u_albedo", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));
    blueMat->setProperty("u_roughness", 0.2f);


    Material* greyMat = assetLibrary.createMaterial("greyMat", "pbr");
    greyMat->setProperty("u_albedo", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    greyMat->setProperty("u_roughness", 0.2f);

    Material* spitfireMat = assetLibrary.createMaterial("spitfireMat", "pbr");
    spitfireMat->setTexture("u_albedoTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_d.png").handle, 0);
    spitfireMat->setTexture("u_metallicTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_m.png").handle, 0);
    spitfireMat->setTexture("u_roughnessTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_r.png").handle, 0);
    spitfireMat->setTexture("u_aoTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_ao.png").handle, 0);

    Material* metalBoxMat = assetLibrary.createMaterial("metalBoxMat", "pbr");
    metalBoxMat->setTexture("u_albedoTex", assetManager.getTexture2D("resources/textures/metalbox/metalbox_a.png").handle, 0);
    metalBoxMat->setTexture("u_roughnessTex", assetManager.getTexture2D("resources/textures/metalbox/metalbox_r.png").handle, 0);
    metalBoxMat->setTexture("u_aoTex", assetManager.getTexture2D("resources/textures/metalbox/metalbox_ao.png").handle, 0);

    Material* cerberusMat = assetLibrary.createMaterial("cerberusMat", "pbr");
    cerberusMat->setTexture("u_albedoTex", assetManager.getTexture2D("resources/textures/cerberus/cerberus_a.png").handle, 0);
    cerberusMat->setTexture("u_metallicTex", assetManager.getTexture2D("resources/textures/cerberus/cerberus_r.png").handle, 0);
    cerberusMat->setTexture("u_roughnessTex", assetManager.getTexture2D("resources/textures/cerberus/cerberus_r.png").handle, 0);

    Material* skyboxMat = assetLibrary.createMaterial("skyboxMat", "skytriangle");
    skyboxMat->setTexture("u_mainTex", skyboxTex, 0);
}

void PbrScene::loadModels(AssetLibrary &assetLibrary)
{
    //assetLibrary.loadModel("sponza", "resources/sponza/sponza.obj", true);
}

void PbrScene::loadLights(ecs::Coordinator& coordinator)
{
    ecs::Entity light = coordinator.createEntity();
    coordinator.addComponent(light, ecs::Light{
        .color = glm::vec3(0.9, 0.9, 0.8),
    });
    coordinator.addComponent(light, ecs::Transform{
        .rotation = glm::vec3(-45.0, 20.0, 0.0),
        .quaternion =  glm::quat(glm::radians(glm::vec3(-45.0, 20.0, 0.0))),
    });
    coordinator.addComponent(light, ecs::Tag{
        .name = "Directional Light"
    });
}

void PbrScene::loadSkybox(AssetLibrary &assetLibrary, AssetManager& assetManager)
{
    Mesh* cubeMap = assetManager.getMesh(MeshType::TriangleMap);
    Material* skyboxMat = assetLibrary.getMaterial("skyboxMat");

}

void PbrScene::loadObjects(AssetLibrary& assetLibrary,
                           AssetManager& assetManager,
                           ecs::Coordinator& coordinator)
{

    Mesh* cube = assetManager.getMesh(MeshType::Cube);
    Mesh* quad = assetManager.getMesh(MeshType::Quad);
    Mesh* sphere = assetManager.getMesh(MeshType::Sphere);
    Model* spitfire = assetManager.getModel(SampleResources::model_spitfire);
    Model* cerberus = assetManager.getModel(SampleResources::model_cerberus);
    Model* sponza = assetManager.getModel("resources/sponza/sponza.obj", true);

    Material* greyMat = assetLibrary.getMaterial("greyMat");
    Material* blueMat = assetLibrary.getMaterial("blueMat");
    Material* spitfireMat = assetLibrary.getMaterial("spitfireMat");
    Material* woodBoxMat = assetLibrary.getMaterial("metalBoxMat");
    Material* cerberusMat = assetLibrary.getMaterial("cerberusMat");

    auto sphereEntity = coordinator.createEntity();
    coordinator.addComponent(sphereEntity, ecs::MeshRenderer{
        .mesh = sphere,
        .material = blueMat,
    });
    coordinator.addComponent(sphereEntity, ecs::Transform{
        .position = SampleResources::object_positions[1],
    });
    coordinator.addComponent(sphereEntity, ecs::Tag{
        .name = "Sphere",
    });
 
    auto cubeEntity = coordinator.createEntity();
    coordinator.addComponent(cubeEntity, ecs::MeshRenderer{
        .mesh = cube,
        .material = woodBoxMat,
    });
    coordinator.addComponent(cubeEntity, ecs::Transform{
        .position = SampleResources::object_positions[3],
    });
    coordinator.addComponent(cubeEntity, ecs::Tag{
        .name = "Cube",
    });

    auto quadEntity = coordinator.createEntity();
    coordinator.addComponent(quadEntity, ecs::MeshRenderer{
        .mesh = quad,
        .material = greyMat,
    });
    coordinator.addComponent(quadEntity, ecs::Transform{
        .position = SampleResources::object_positions[0],
        .rotation = glm::vec3(-90.0, 0.0, 0.0),
        .scale = glm::vec3(10.0, 10.0, 10.0),
    });
    coordinator.addComponent(quadEntity, ecs::Tag{
        .name = "Quad",
    });

    auto spitfireEntity = coordinator.createEntity();
    coordinator.addComponent(spitfireEntity, ecs::Transform{});
    coordinator.addComponent(spitfireEntity, ecs::Tag{
        .name = "Spitfire",
    });

    auto transformSystem = coordinator.getSytem<TransformSystem>();

    for (auto& mesh : spitfire->m_meshes)
    {
        auto newEntity = coordinator.createEntity();
        coordinator.addComponent(newEntity, ecs::Transform{});
        coordinator.addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = spitfireMat,
        });
        coordinator.addComponent(newEntity, ecs::Tag{
            .name = mesh.first,
        });

        transformSystem->addChild(spitfireEntity, newEntity);
    }

    auto& spitfireTransform = coordinator.getComponent<ecs::Transform>(
            spitfireEntity);
    spitfireTransform.position = glm::vec3( 2.4f, -6.5f, -3.5f);
    spitfireTransform.rotation = glm::vec3(-96.0, 0.0, 0.0);
    spitfireTransform.scale = glm::vec3(0.05, 0.05, 0.05);


    auto cerberusEntity = coordinator.createEntity();
    coordinator.addComponent(cerberusEntity, ecs::Transform{});
    coordinator.addComponent(cerberusEntity, ecs::Tag{
        .name = "Cerberus",
    });

    for (auto& mesh : cerberus->m_meshes)
    {
        auto newEntity = coordinator.createEntity();
        coordinator.addComponent(newEntity, ecs::Transform{});
        coordinator.addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = cerberusMat,
        });
        coordinator.addComponent(newEntity, ecs::Tag{
            .name = mesh.first,
        });
        transformSystem->addChild(cerberusEntity, newEntity);
    }

    auto& cerberusTransform = coordinator.getComponent<ecs::Transform>(
            cerberusEntity);
    cerberusTransform.position = SampleResources::object_positions[5];
    cerberusTransform.rotation = glm::vec3(-96.0, 0.0, 0.0);
    cerberusTransform.scale = glm::vec3(0.05);

    auto sponzaEntity = coordinator.createEntity();
    coordinator.addComponent(sponzaEntity, ecs::Transform{});
    coordinator.addComponent(sponzaEntity, ecs::Tag{
        .name = "Sponza",
    });

    for (auto& mesh : sponza->m_meshes)
    {
        auto newEntity = coordinator.createEntity();
        coordinator.addComponent(newEntity, ecs::Transform{});
        coordinator.addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = AssetLibrary::instance().getMaterial(mesh.second->modelMat().c_str()),
        });
        coordinator.addComponent(newEntity, ecs::Tag{
            .name = mesh.first,
        });
        transformSystem->addChild(sponzaEntity, newEntity);
    }

    auto& sponzaTransform = coordinator.getComponent<ecs::Transform>(sponzaEntity);
    sponzaTransform.scale = glm::vec3(0.01, 0.01, 0.01);

    transformSystem->update();
    transformSystem->addChild(cubeEntity, sphereEntity);
    transformSystem->addChild(quadEntity, cubeEntity);
    transformSystem->updateHierarchically();
}
