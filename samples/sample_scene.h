//
// Created by Diego Azulay on 16/04/22.
//

#ifndef INTERACTIVEGRAPHICS_SAMPLE_SCENE_H
#define INTERACTIVEGRAPHICS_SAMPLE_SCENE_H


#include <unordered_map>
#include "../scene.h"
#include "../assets/shader.h"
#include "../assets/material.h"
#include "../asset_library.h"

class SampleScene
{
public:
    SampleScene(Scene* scene);

private:
    void createIrradianceMaps();
    void createShaders();
    void createMaterials();
    void loadMeshes();
    void addSceneObjects();
    void addLights();
    void addSkybox();

    Scene* m_scene;
    AssetLibrary m_assetLibrary;

    glm::vec3 m_cubePositions[10] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    unsigned int hdrTexture;
};


#endif //INTERACTIVEGRAPHICS_SAMPLE_SCENE_H
