#include "pbr_scene.h"
#include "sample_resources.h"
#include "../entity_factory.h"
#include "../components/transform.h"
#include "../irradiance_map_factory.h"

void PbrScene::loadScene(Scene &scene, AssetLibrary &assetLibrary)
{
    unsigned int skyboxTex;

    loadShaders(assetLibrary);
    loadTextures(assetLibrary);
    loadIrradianceTextures(scene, assetLibrary, skyboxTex);
    loadMaterials(assetLibrary, skyboxTex);
    loadModels(assetLibrary);
    loadLights(scene);
    loadSkybox(scene, assetLibrary);
    loadObjects(scene, assetLibrary);
}

void PbrScene::loadShaders(AssetLibrary &assetLibrary)
{
    assetLibrary.loadShader("lambert", SampleResources::shader_lambert);
    assetLibrary.loadShader("pbr", SampleResources::shader_pbr);
    assetLibrary.loadShader("skybox", SampleResources::shader_skybox);
}

void PbrScene::loadTextures(AssetLibrary& assetLibrary)
{
    assetLibrary.load2DTexture("spitfire", SampleResources::texture_spitfire, SampleResources::texture_spitfire_dir);
    assetLibrary.load2DTexture("spitfire_m", SampleResources::texture_spitfire_metallic, SampleResources::texture_spitfire_dir);
    assetLibrary.load2DTexture("spitfire_r", SampleResources::texture_spitfire_roughness, SampleResources::texture_spitfire_dir);
    assetLibrary.load2DTexture("spitfire_ao", SampleResources::texture_spitfire_ao, SampleResources::texture_spitfire_dir);
    assetLibrary.load2DTexture("woodBox", SampleResources::texture_woodBox, SampleResources::texture_dir);
    assetLibrary.loadHDRTexture("tiber", SampleResources::texture_tiber, SampleResources::texture_skybox_dir);
}

void PbrScene::loadIrradianceTextures(Scene& scene, AssetLibrary& assetLibrary, unsigned int& skyboxTex)
{

    IrradianceMaps maps = IrradianceMapFactory::generateIrradianceMapsFromHDR(assetLibrary.getTexture("tiber")->ID());

    scene.irradianceMap = maps.irradianceMap;
    scene.prefilterMap = maps.prefilterMap;
    scene.brdfLUT = maps.brdfLUTMap;
    skyboxTex = maps.cubeMap;
}

void PbrScene::loadMaterials(AssetLibrary &assetLibrary, unsigned int& skyboxTex)
{
    Material* blueMat = assetLibrary.createMaterial("blueMat", "pbr");
    blueMat->setProperty("u_albedo", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));
    blueMat->setProperty("u_metallic", 1.0f);
    blueMat->setProperty("u_roughness", 0.2f);


    Material* greyMat = assetLibrary.createMaterial("greyMat", "lambert");
    greyMat->setProperty("u_color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    Material* spitfireMat = assetLibrary.createMaterial("spitfireMat", "pbr");
    spitfireMat->setTexture("u_albedoTex", assetLibrary.getTexture("spitfire")->ID(), 0);
    spitfireMat->setTexture("u_metallicTex", assetLibrary.getTexture("spitfire_m")->ID(), 0);
    spitfireMat->setTexture("u_roughnessTex", assetLibrary.getTexture("spitfire_r")->ID(), 0);
    spitfireMat->setTexture("u_aoTex", assetLibrary.getTexture("spitfire_ao")->ID(), 0);
    spitfireMat->setProperty("u_metallic", 1.0f);
    spitfireMat->setProperty("u_roughness", 1.0f);


    Material* woodBoxMat = assetLibrary.createMaterial("woodBoxMat", "pbr");
    woodBoxMat->setTexture("u_albedoTex", assetLibrary.getTexture("woodBox")->ID(), 0);
    woodBoxMat->setProperty("u_roughness", 1.0f);

    Material* skyboxMat = assetLibrary.createMaterial("skyboxMat", "skybox");
    skyboxMat->setTexture("u_mainTex", skyboxTex, 0);
}

void PbrScene::loadModels(AssetLibrary &assetLibrary)
{
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::Cube);
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::Quad);
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::Sphere);
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::CubeMapModel);

    assetLibrary.loadModel("spitfireModel", SampleResources::model_spitfire);
}

void PbrScene::loadLights(Scene& scene)
{
    scene.addLight(EntityFactory::createDirectionalLight(glm::vec3(-45.0f, 0.0f, 0.0f), glm::vec3(0.9f, 0.9f, 0.8f), 1.0f));
}

void PbrScene::loadSkybox(Scene &scene, AssetLibrary &assetLibrary)
{
    Mesh* cubeMap = assetLibrary.getMesh("cubeMap");
    Material* skyboxMat = assetLibrary.getMaterial("skyboxMat");

    scene.addSkybox(EntityFactory::createFromMesh(
            glm::vec3(0.0, 0.0, 0.0), skyboxMat, cubeMap));
}

void PbrScene::loadObjects(Scene& scene, AssetLibrary& assetLibrary)
{
    Mesh* cube = assetLibrary.getMesh("cube");
    Mesh* quad = assetLibrary.getMesh("quad");
    Mesh* sphere = assetLibrary.getMesh("sphere");
    Model* spitfire = assetLibrary.getModel("spitfireModel");

    Material* greyMat = assetLibrary.getMaterial("greyMat");
    Material* blueMat = assetLibrary.getMaterial("blueMat");
    Material* spitfireMat = assetLibrary.getMaterial("spitfireMat");
    Material* woodBoxMat = assetLibrary.getMaterial("woodBoxMat");

    Entity e = EntityFactory::createFromMesh(
            SampleResources::object_positions[0], greyMat, quad);

    auto transform = e.getComponent<Transform>();
    transform->eulerAngles(glm::vec3(-90.0, 0.0, 0.0));
    transform->scale(glm::vec3(10.0, 10.0, 10.0));
    transform->updateModelMatrix();

    scene.addObject(e);

    scene.addObject(EntityFactory::createFromMesh(
            SampleResources::object_positions[1], blueMat, sphere));

    scene.addObject(EntityFactory::createFromMesh(
            SampleResources::object_positions[3], woodBoxMat, cube));

    Entity spitfireEntity = EntityFactory::createFromModel(
            SampleResources::object_positions[4], spitfireMat,spitfire);

    auto t = spitfireEntity.getComponent<Transform>();
    t->scale(glm::vec3(0.05, 0.05, 0.05));
    t->eulerAngles(glm::vec3(-96, 0, 0));
    t->updateModelMatrix();
    spitfireEntity.updateSelfAndChild();

    scene.addObject(spitfireEntity);
}