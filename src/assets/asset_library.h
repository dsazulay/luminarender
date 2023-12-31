#pragma once

#include "shader.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"

#include <vector>
#include <unordered_map>
#include <string>

class AssetLibrary
{
public:
    struct DefaultResources
    {
        static constexpr char* texWhite = (char*) "defaultWhite";
    };

    AssetLibrary(const AssetLibrary& other) = delete;
    void operator=(const AssetLibrary& other) = delete;
    ~AssetLibrary();

    static AssetLibrary& instance();

    Shader* loadShader(const char* name, const char* shaderPath);
    Material* createMaterial(const char* name, const char* shader);
    Material* createMaterial(const char* name, Shader*);
    Model* loadModel(const char* name, const char* path, bool material);
    Mesh* loadMesh(const char* name, MeshType basicModel);
    Texture* load2DTexture(const char* name, const std::string& file, const std::string& directory);
    Texture* loadHDRTexture(const char* name, const std::string& file, const std::string& directory);
    Texture* loadCubeMapTexture(const char* name, const std::vector<std::string> faces, const std::string& directory);

    void createDefaultResources();

    Shader* getShader(const char* name);
    Material* getMaterial(const char* name);
    Mesh* getMesh(const char* name);
    Model* getModel(const char* name);
    Texture* getTexture(const char* name);

private:
    AssetLibrary();
    void loadDefaultResources();
    void setMaterialDefaultResources();
    void generateSSAONoiseTexture();

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

    static constexpr char* texture_defaultWhite = (char*) "default_white.png";
    static constexpr char* texture_dir = (char*) "resources/textures";
};
