#include "pbr_scene.h"

#include "sample_resources.h"
#include "irradiance_map_factory.h"
#include "components/components.h"
#include "renderer/transform_system.h"


void PbrScene::loadScene(AssetManager& assetManager,
                         ecs::Coordinator& coordinator)
{
    unsigned int skyboxTex;

    loadIrradianceTextures(skyboxTex, assetManager);
    loadMaterials(assetManager, skyboxTex);
    loadLights(coordinator);
    loadSkybox(assetManager);
    loadObjects(assetManager, coordinator);
}


void PbrScene::loadIrradianceTextures(unsigned int& skyboxTex,
                                      AssetManager& assetManager)
{

    IrradianceMaps maps = IrradianceMapFactory::generateIrradianceMapsFromHDR(assetManager.getTextureHDR("resources/textures/skybox/tiber_2.hdr").handle, assetManager);

    skyboxTex = maps.cubeMap;
}

void PbrScene::loadMaterials(AssetManager &assetManager, unsigned int& skyboxTex)
{
    Shader* pbr = &assetManager.getShader("resources/shaders/cook_torrance.glsl");
    Material& blueMat = assetManager.createMaterial("blueMat", pbr);
    blueMat.setColor("u_albedo", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));
    blueMat.setFloat("u_roughness", 0.9f);


    Material& greyMat = assetManager.createMaterial("greyMat", pbr);
    greyMat.setColor("u_albedo", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    greyMat.setFloat("u_roughness", 0.2f);

    Material& spitfireMat = assetManager.createMaterial("spitfireMat", pbr);
    spitfireMat.setTexture("u_albedoTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_d.png").handle);
    spitfireMat.setTexture("u_metallicTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_m.png").handle);
    spitfireMat.setTexture("u_roughnessTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_r.png").handle);
    spitfireMat.setTexture("u_aoTex", assetManager.getTexture2D("resources/textures/spitfire/spitfire_ao.png").handle);

    Material& metalBoxMat = assetManager.createMaterial("metalBoxMat", pbr);
    metalBoxMat.setTexture("u_albedoTex", assetManager.getTexture2D("resources/textures/metalbox/metalbox_a.png").handle);
    metalBoxMat.setTexture("u_roughnessTex", assetManager.getTexture2D("resources/textures/metalbox/metalbox_r.png").handle);
    metalBoxMat.setTexture("u_aoTex", assetManager.getTexture2D("resources/textures/metalbox/metalbox_ao.png").handle);

    Material& cerberusMat = assetManager.createMaterial("cerberusMat", pbr);
    cerberusMat.setTexture("u_albedoTex", assetManager.getTexture2D("resources/textures/cerberus/cerberus_a.png").handle);
    cerberusMat.setTexture("u_metallicTex", assetManager.getTexture2D("resources/textures/cerberus/cerberus_r.png").handle);
    cerberusMat.setTexture("u_roughnessTex", assetManager.getTexture2D("resources/textures/cerberus/cerberus_r.png").handle);

    Shader* skytri = &assetManager.getShader("resources/shaders/skytriangle.glsl");
    Material& skyboxMat = assetManager.createMaterial("skyboxMat", skytri);
    skyboxMat.setTexture("u_mainTex", skyboxTex);
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

void PbrScene::loadSkybox(AssetManager& assetManager)
{
    Mesh cubeMap = assetManager.getMesh(MeshType::TriangleMap);
    Material& skyboxMat = assetManager.getMaterial("skyboxMat");

}

void PbrScene::loadObjects(AssetManager& assetManager,
                           ecs::Coordinator& coordinator)
{

    Mesh cube = assetManager.getMesh(MeshType::Cube);
    Mesh quad = assetManager.getMesh(MeshType::Quad);
    Mesh sphere = assetManager.getMesh(MeshType::Sphere);
    const Model& spitfire = assetManager.getModel(SampleResources::model_spitfire);
    const Model& cerberus = assetManager.getModel(SampleResources::model_cerberus);
    const Model& sponza = assetManager.getModel("resources/sponza/sponza.obj", true);

    Material& greyMat = assetManager.getMaterial("greyMat");
    Material& blueMat = assetManager.getMaterial("blueMat");
    Material& spitfireMat = assetManager.getMaterial("spitfireMat");
    Material& woodBoxMat = assetManager.getMaterial("metalBoxMat");
    Material& cerberusMat = assetManager.getMaterial("cerberusMat");

    auto sphereEntity = coordinator.createEntity();
    coordinator.addComponent(sphereEntity, ecs::MeshRenderer{
        .mesh = sphere,
        .material = &blueMat,
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
        .material = &woodBoxMat,
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
        .material = &greyMat,
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

    for (auto& mesh : spitfire.meshes)
    {
        auto newEntity = coordinator.createEntity();
        coordinator.addComponent(newEntity, ecs::Transform{});
        coordinator.addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = &spitfireMat,
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

    for (auto& mesh : cerberus.meshes)
    {
        auto newEntity = coordinator.createEntity();
        coordinator.addComponent(newEntity, ecs::Transform{});
        coordinator.addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = &cerberusMat,
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

    for (auto& mesh : sponza.meshes)
    {
        auto newEntity = coordinator.createEntity();
        coordinator.addComponent(newEntity, ecs::Transform{});
        coordinator.addComponent(newEntity, ecs::MeshRenderer{
            .mesh = mesh.second,
            .material = &assetManager.getMaterial(mesh.second.importedMatName.c_str()),
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
