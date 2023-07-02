#include "lighting_skybox_scene.h"
#include "sample_resources.h"
#include "../entity_factory.h"
#include "../components/transform.h"
#include <utility>

void LightingSkyboxScene::loadScene(Scene &scene, AssetLibrary &assetLibrary)
{
    loadShaders(assetLibrary);
    loadTextures(assetLibrary);
    loadMaterials(assetLibrary);
    loadModels(assetLibrary);
    loadLights(scene);
    loadSkybox(scene, assetLibrary);
    loadObjects(scene, assetLibrary);
}

void LightingSkyboxScene::loadShaders(AssetLibrary &assetLibrary)
{
    assetLibrary.loadShader("lambert", SampleResources::shader_lambert);
    assetLibrary.loadShader("skybox", SampleResources::shader_skybox);
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

    Material* skyboxMat = assetLibrary.createMaterial("skyboxMat", "skybox");
    skyboxMat->setTexture("u_mainTex", assetLibrary.getTexture("glaciers")->ID(), 0);
}

void LightingSkyboxScene::loadModels(AssetLibrary &assetLibrary)
{
    assetLibrary.loadMesh(BasicMeshType::Cube);
    assetLibrary.loadMesh(BasicMeshType::Quad);
    assetLibrary.loadMesh(BasicMeshType::Sphere);
    assetLibrary.loadMesh(BasicMeshType::CubeMap);

    assetLibrary.loadModel("spitfireModel", SampleResources::model_spitfire);
}

void LightingSkyboxScene::loadLights(Scene& scene)
{
    scene.addLight(EntityFactory::createDirectionalLight("Directional Light", glm::vec3(-45.0f, 20.0f, 0.0f), glm::vec3(0.9f, 0.9f, 0.8f), 1.0f));
    // scene.addLight(EntityFactory::createPointLight("Point Light", glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.8f, 0.5f), 1.0f));
    // scene.addLight(EntityFactory::createSpotLight("Spot Light", glm::vec3(-1.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.5f, 0.2f), 1.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f))));
    scene.mainLight(scene.lights().front().get());
}

void LightingSkyboxScene::loadSkybox(Scene &scene, AssetLibrary &assetLibrary)
{
    Mesh* cubeMap = assetLibrary.getMesh("cubeMap");
    Material* skyboxMat = assetLibrary.getMaterial("skyboxMat");

    scene.addSkybox(EntityFactory::createFromMesh("Skybox",
            glm::vec3(0.0, 0.0, 0.0), skyboxMat, cubeMap));
}

void LightingSkyboxScene::loadObjects(Scene& scene, AssetLibrary& assetLibrary)
{
    Mesh* cube = assetLibrary.getMesh("cube");
    Mesh* quad = assetLibrary.getMesh("quad");
    Mesh* sphere = assetLibrary.getMesh("sphere");
    Model* spitfire = assetLibrary.getModel("spitfireModel");

    Material* greyMat = assetLibrary.getMaterial("greyMat");
    Material* blueMat = assetLibrary.getMaterial("blueMat");
    Material* spitfireMat = assetLibrary.getMaterial("spitfireMat");
    Material* woodBoxMat = assetLibrary.getMaterial("woodBoxMat");

    auto e = EntityFactory::createFromMesh("Quad",
            SampleResources::object_positions[0], greyMat, quad);

    auto transform = e->getComponent<Transform>();
    transform->eulerAngles(glm::vec3(-90.0, 0.0, 0.0));
    transform->scale(glm::vec3(10.0, 10.0, 10.0));
    transform->updateModelMatrix();


    auto sphereEntity = EntityFactory::createFromMesh("Sphere",
            SampleResources::object_positions[1], blueMat, sphere);

    auto cubeEntity = EntityFactory::createFromMesh("Cube",
            SampleResources::object_positions[3], woodBoxMat, cube);

    cubeEntity->addChild(std::move(sphereEntity));
    e->addChild(std::move(cubeEntity));
    e->updateSelfAndChild();
    scene.addObject(std::move(e));

    auto spitfireEntity = EntityFactory::createFromModel("Spitfire",
            SampleResources::object_positions[4], spitfireMat,spitfire);

    auto t = spitfireEntity->getComponent<Transform>();
    t->scale(glm::vec3(0.05, 0.05, 0.05));
    t->eulerAngles(glm::vec3(-96, 0, 0));
    t->updateModelMatrix();
    spitfireEntity->updateSelfAndChild();

    scene.addObject(std::move(spitfireEntity));
}


