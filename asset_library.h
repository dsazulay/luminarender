//
// Created by Diego Azulay on 18/04/22.
//

#ifndef INTERACTIVEGRAPHICS_ASSET_LIBRARY_H
#define INTERACTIVEGRAPHICS_ASSET_LIBRARY_H


#include <unordered_map>
#include "shader.h"
#include "material.h"
#include "mesh.h"
#include "model.h"

class AssetLibrary
{
public:
    enum TextureType
    {
        Tex2D,
        CubeMap,
        HDR
    };

    enum BasicModel
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
//    int loadTexture(const char* name, TextureType type, const std::string& file, const std::string& directory);
    Model* loadModel(const char* name, const char* path);
    Mesh* loadModel(BasicModel basicModel);

    Shader* getShader(const char* name);
    Material* getMaterial(const char* name);
    Mesh* getMesh(const char* name);
    Model* getModel(const char* name);

private:
//    AssetLibrary() = default;
//    AssetLibrary(const AssetLibrary& other) = delete;
//    void operator=(const AssetLibrary& other) = delete;

    bool isShaderLoaded(const char* name);
    bool isMaterialCreated(const char* name);
    bool isMeshLoaded(const char* name);
    bool isModelLoaded(const char* name);

    std::unordered_map<std::string, Shader*> m_shaders;
    std::unordered_map<std::string, Material*> m_materials;
    std::unordered_map<std::string, Mesh*> m_meshes;
    std::unordered_map<std::string, Model*> m_models;
};

#endif //INTERACTIVEGRAPHICS_ASSET_LIBRARY_H
