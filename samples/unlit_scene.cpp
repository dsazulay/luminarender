#include "unlit_scene.h"
#include "sample_resources.h"
#include "../entity_factory.h"
#include "../components/transform.h"

void UnlitScene::loadScene(Scene &scene, AssetLibrary &assetLibrary)
{
    loadShaders(assetLibrary);
    loadTextures(assetLibrary);
    loadMaterials(assetLibrary);
    loadModels(assetLibrary);
    loadObjects(scene, assetLibrary);
}

void UnlitScene::loadShaders(AssetLibrary &assetLibrary)
{
    assetLibrary.loadShader("unlit", SampleResources::shader_unlit);
}

void UnlitScene::loadTextures(AssetLibrary& assetLibrary)
{
    assetLibrary.load2DTexture("defaultWhite", SampleResources::texture_defaultWhite, SampleResources::texture_dir);
    assetLibrary.load2DTexture("container", SampleResources::texture_container, SampleResources::texture_dir);
    assetLibrary.load2DTexture("woodBox", SampleResources::texture_woodBox, SampleResources::texture_dir);
}

void UnlitScene::loadMaterials(AssetLibrary &assetLibrary)
{
    Material* blueMat = assetLibrary.createMaterial("blueMat", "unlit");
    blueMat->setProperty("u_color", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));
    blueMat->setTexture("u_mainTex", assetLibrary.getTexture("defaultWhite")->ID(), 0);

    Material* greyMat = assetLibrary.createMaterial("greyMat", "unlit");
    greyMat->setProperty("u_color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    greyMat->setTexture("u_mainTex", assetLibrary.getTexture("defaultWhite")->ID(), 0);

    Material* containerMat = assetLibrary.createMaterial("containerMat", "unlit");
    containerMat->setProperty("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    containerMat->setTexture("u_mainTex", assetLibrary.getTexture("container")->ID(), 0);

    Material* woodBoxMat = assetLibrary.createMaterial("woodBoxMat", "unlit");
    woodBoxMat->setProperty("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    woodBoxMat->setTexture("u_mainTex", assetLibrary.getTexture("woodBox")->ID(), 0);
}

void UnlitScene::loadModels(AssetLibrary &assetLibrary)
{
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::Cube);
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::Quad);
    assetLibrary.loadMesh(AssetLibrary::BasicMesh::Sphere);
}

void UnlitScene::loadObjects(Scene& scene, AssetLibrary& assetLibrary)
{
    Mesh* cube = assetLibrary.getMesh("cube");
    Mesh* quad = assetLibrary.getMesh("quad");
    Mesh* sphere = assetLibrary.getMesh("sphere");

    Material* greyMat = assetLibrary.getMaterial("greyMat");
    Material* blueMat = assetLibrary.getMaterial("blueMat");
    Material* containerMat = assetLibrary.getMaterial("containerMat");
    Material* woodBoxMat = assetLibrary.getMaterial("woodBoxMat");

    Entity e = EntityFactory::createFromMesh(
            SampleResources::object_positions[0], greyMat, quad);

    auto transform = e.getComponent<Transform>();
    transform->eulerAngles(glm::vec3(-90.0, 0.0, 0.0));
    transform->scale(glm::vec3(5.0, 5.0, 5.0));
    transform->updateModelMatrix();

    scene.addObject(e);

    scene.addObject(EntityFactory::createFromMesh(
            SampleResources::object_positions[1], blueMat, sphere));

    scene.addObject(EntityFactory::createFromMesh(
            SampleResources::object_positions[2],containerMat, cube));

    scene.addObject(EntityFactory::createFromMesh(
            SampleResources::object_positions[3], woodBoxMat, cube));
}
