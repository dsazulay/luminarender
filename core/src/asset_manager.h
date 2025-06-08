#pragma once

#include "assets/model_types.h"
#include "file_view.h"
#include "assets/mesh.h"
#include "assets/model.h"
//#include "shader.h"
#include "assets/texture.h"
#include "renderer/gpuresourcemanager.h"
#include "renderer/opengl.h"
#include "assets/importer.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

struct AssetFile
{
    std::string path;
    std::filesystem::file_time_type lastWriteTime;
    FileView handle;
};

class AssetManager
{
public:
    Texture* getTexture2D(std::string_view path);
    Texture* getTextureHDR(std::string_view path);
    Texture* getTextureCM(std::vector<std::string> path);
    Mesh* getMesh(MeshType type);
    Mesh* getMesh(std::string_view name);
    Model* getModel(std::string_view path, bool loadMaterial = false);
    //Shader& getShader(std::string_view path);
    void loadDefaultResources();
    void checkFileModification();

private:
    std::vector<AssetFile> m_files;

    Texture* loadTexture(TextureData &textureData);
    Mesh* loadMesh(MeshType type, VertexIndexTuple& mesh);

    std::unordered_map<std::string, Texture*> m_textures;
    std::unordered_map<std::string, Model*> m_models;
    std::unordered_map<std::string, Mesh*> m_importedModels;
    std::unordered_map<MeshType, Mesh*> m_nativeModels;
    //std::unordered_map<std::string, Shader> m_shaders;

    GPUResourceManager<OpenGL>* m_gpurm{};
};

AssetFile createAssetFile(std::string_view path, FileView fileView);
