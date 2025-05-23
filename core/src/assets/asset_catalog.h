#pragma once

#include "file_view.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "../renderer/gpuresourcemanager.h"
#include "../renderer/opengl.h"

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

class AssetCatalog
{
  public:
    //Texture& getTexture(std::string_view path);
    Mesh* getModel(MeshType type);
    //Model& getModel(std::string_view path);
    //Shader& getShader(std::string_view path);
    void loadDefaultResources();
    void checkFileModification();

  private:
    std::vector<AssetFile> m_files;

    //std::unordered_map<std::string, Texture> m_textures;
    //std::unordered_map<std::string, Model> m_models;
    std::unordered_map<MeshType, Mesh*> m_nativeModels;
    //std::unordered_map<std::string, Shader> m_shaders;

    GPUResourceManager<OpenGL> m_gpurm{};
};

AssetFile createAssetFile(std::string_view path, FileView fileView);
