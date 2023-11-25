#include "lighting_skybox_scene.h"

#include "sample_resources.h"
#include "../entity_factory.h"
#include "../components/transform.h"
#include "../components/components.h"
#include "../renderer/transform_system.h"


void LightingSkyboxScene::loadScene(Scene &scene, AssetLibrary &assetLibrary, ecs::Coordinator& coordinator)
{
    loadTextures(assetLibrary);
    loadMaterials(assetLibrary);
    loadModels(assetLibrary);
    loadLights(scene, coordinator);
    loadSkybox(scene, assetLibrary);
    loadObjects(scene, assetLibrary, coordinator);
}

void LightingSkyboxScene::loadTextures(AssetLibrary& assetLibrary)
{
    std::vector<std::string> faces =
    {
        SampleResources::faces[0], SampleResources::faces[1], SampleResources::faces[2],
        SampleResources::faces[3], SampleResources::faces[4], SampleResources::faces[5],
    };

    assetLibrary.load2DTexture("spitfire", SampleResources::texture_spitfire, SampleResources::texture_spitfire_dir);
    assetLibrary.load2DTexture("woodBox", SampleResources::texture_woodBox, SampleResources::texture_dir);
    assetLibrary.loadCubeMapTexture("glaciers", faces, SampleResources::texture_skybox_dir);
}

void LightingSkyboxScene::loadMaterials(AssetLibrary &assetLibrary)
{
    Material* blueMat = assetLibrary.createMaterial("blueMat", "lambert");
    blueMat->setProperty("u_color", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));

    Material* greyMat = assetLibrary.createMaterial("greyMat", "lambert");
    greyMat->setProperty("u_color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    Material* spitfireMat = assetLibrary.createMaterial("spitfireMat", "lambert");
    spitfireMat->setTexture("u_mainTex", assetLibrary.getTexture("spitfire")->ID(), 0);

    Material* woodBoxMat = assetLibrary.createMaterial("woodBoxMat", "lambert");
    woodBoxMat->setTexture("u_mainTex", assetLibrary.getTexture("woodBox")->ID(), 0);

    Material* skyboxMat = assetLibrary.createMaterial("skyboxMat", "skytriangle");
    skyboxMat->setTexture("u_mainTex", assetLibrary.getTexture("glaciers")->ID(), 0);
}

void LightingSkyboxScene::loadModels(AssetLibrary &assetLibrary)
{
    assetLibrary.loadModel("spitfireModel", SampleResources::model_spitfire, false);
    assetLibrary.loadModel("sponza", "resources/sponza/sponza.obj", true);
}

void LightingSkyboxScene::loadLights(Scene& scene, ecs::Coordinator& coordinator)
{
    // scene.addLight(EntityFactory::createPointLight("Point Light", glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.8f, 0.5f), 1.0f));
    // scene.addLight(EntityFactory::createSpotLight("Spot Light", glm::vec3(-1.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.5f, 0.2f), 1.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f))));
    ecs::Entity light = coordinator.createEntity();
    coordinator.addComponent(light, ecs::Light{
        .color = glm::vec3(0.9, 0.9, 0.8),
    });
    coordinator.addComponent(light, ecs::Transform{
        .rotation = glm::vec3(-45.0, 20.0, 0.0),
        .quaternion =  glm::quat(glm::radians(glm::vec3(-45.0, 20.0, 0.0))),
    });
}

void LightingSkyboxScene::loadSkybox(Scene &scene, AssetLibrary &assetLibrary)
{
    Mesh* cubeMap = assetLibrary.getMesh("triangleMap");
    Material* skyboxMat = assetLibrary.getMaterial("skyboxMat");

    scene.addSkybox(EntityFactory::createFromMesh("Skybox",
            glm::vec3(0.0, 0.0, 0.0), skyboxMat, cubeMap));
}

void LightingSkyboxScene::loadObjects(Scene& scene, AssetLibrary& assetLibrary, ecs::Coordinator& coordinator)
{
    Mesh* cube = assetLibrary.getMesh("cube");
    Mesh* quad = assetLibrary.getMesh("quad");
    Mesh* sphere = assetLibrary.getMesh("sphere");
    Model* spitfire = assetLibrary.getModel("spitfireModel");
    Model* sponza = assetLibrary.getModel("sponza");

    Material* greyMat = assetLibrary.getMaterial("greyMat");
    Material* blueMat = assetLibrary.getMaterial("blueMat");
    Material* woodBoxMat = assetLibrary.getMaterial("woodBoxMat");
    Material* spitfireMat = assetLibrary.getMaterial("spitfireMat");

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

    auto& spitfireTransform = coordinator.getComponent<ecs::Transform>(spitfireEntity);
    spitfireTransform.position = glm::vec3( 2.4f, -6.5f, -3.5f);
    spitfireTransform.rotation = glm::vec3(-96.0, 0.0, 0.0);
    spitfireTransform.scale = glm::vec3(0.05, 0.05, 0.05);

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

