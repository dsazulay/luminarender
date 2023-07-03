#include "unlit_scene.h"
#include "sample_resources.h"
#include "../entity_factory.h"
#include "../components/transform.h"

void UnlitScene::loadScene(Scene &scene, AssetLibrary &assetLibrary)
{
    loadTextures(assetLibrary);
    loadMaterials(assetLibrary);
    loadObjects(scene, assetLibrary);
}

void UnlitScene::loadTextures(AssetLibrary& assetLibrary)
{
    assetLibrary.load2DTexture("container", SampleResources::texture_container, SampleResources::texture_dir);
    assetLibrary.load2DTexture("woodBox", SampleResources::texture_woodBox, SampleResources::texture_dir);
}

void UnlitScene::loadMaterials(AssetLibrary &assetLibrary)
{
    Material* blueMat = assetLibrary.createMaterial("blueMat", "unlit");
    blueMat->setProperty("u_color", glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));

    Material* greyMat = assetLibrary.createMaterial("greyMat", "unlit");
    greyMat->setProperty("u_color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    Material* containerMat = assetLibrary.createMaterial("containerMat", "unlit");
    containerMat->setTexture("u_mainTex", assetLibrary.getTexture("container")->ID(), 0);

    Material* woodBoxMat = assetLibrary.createMaterial("woodBoxMat", "unlit");
    woodBoxMat->setTexture("u_mainTex", assetLibrary.getTexture("woodBox")->ID(), 0);
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

    auto e = EntityFactory::createFromMesh("Quad",
            SampleResources::object_positions[0], greyMat, quad);

    auto transform = e->getComponent<Transform>();
    transform->eulerAngles(glm::vec3(-90.0, 0.0, 0.0));
    transform->scale(glm::vec3(5.0, 5.0, 5.0));
    transform->updateModelMatrix();

    scene.addObject(std::move(e));

    scene.addObject(EntityFactory::createFromMesh("Sphere",
            SampleResources::object_positions[1], blueMat, sphere));

    scene.addObject(EntityFactory::createFromMesh("Cube 1",
            SampleResources::object_positions[2],containerMat, cube));

    scene.addObject(EntityFactory::createFromMesh("Cube 2",
            SampleResources::object_positions[3], woodBoxMat, cube));
}
