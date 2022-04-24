//
// Created by Diego Azulay on 18/04/22.
//

#ifndef INTERACTIVEGRAPHICS_ASSET_LIBRARY_H
#define INTERACTIVEGRAPHICS_ASSET_LIBRARY_H


#include <unordered_map>
#include "assets/shader.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/model.h"
#include "assets/texture.h"

class AssetLibrary
{
public:
    enum BasicMesh
    {
        Cube,
        CubeMapModel,
        Quad,
        Sphere
    };

//    static AssetLibrary& instance();
    ~AssetLibrary();

    Shader* loadShader(const char* name, const char* vertPath, const char* fragPath);
    Material* createMaterial(const char* name, const char* shader);
    Material* createMaterial(const char* name, Shader*);
    Model* loadModel(const char* name, const char* path);
    Mesh* loadMesh(BasicMesh basicModel);
    Texture* load2DTexture(const char* name, const std::string& file, const std::string& directory);
    Texture* loadHDRTexture(const char* name, const std::string& file, const std::string& directory);
    Texture* loadCubeMapTexture(const char* name, const std::vector<std::string> faces, const std::string& directory);

    Shader* getShader(const char* name);
    Material* getMaterial(const char* name);
    Mesh* getMesh(const char* name);
    Model* getModel(const char* name);
    Texture* getTexture(const char* name);

private:
//    AssetLibrary() = default;
//    AssetLibrary(const AssetLibrary& other) = delete;
//    void operator=(const AssetLibrary& other) = delete;

    bool isShaderLoaded(const char* name);
    bool isMaterialCreated(const char* name);
    bool isMeshLoaded(const char* name);
    bool isModelLoaded(const char* name);
    bool isTextureLoaded(const char* name);

    std::unordered_map<std::string, Shader*> m_shaders;
    std::unordered_map<std::string, Material*> m_materials;
    std::unordered_map<std::string, Mesh*> m_meshes;
    std::unordered_map<std::string, Model*> m_models;
    std::unordered_map<std::string, Texture*> m_textures;
};

#endif //INTERACTIVEGRAPHICS_ASSET_LIBRARY_H
