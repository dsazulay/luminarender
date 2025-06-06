#include "asset_manager.h"

#include "assets/model_types.h"
#include "log.h"
#include "logger.h"
#include "assets/model.h"
#include "assets/primitives.h"
#include "assets/importer.h"
#include "renderer/gfxapi.h"

#include <cstddef>
#include <utility>

AssetFile createAssetFile(std::string_view path, FileView fileView)
{
    return AssetFile{
        .path = std::string{ path },
        .lastWriteTime = std::filesystem::last_write_time(path),
        .handle = fileView,
    };
}

Texture* AssetManager::getTexture(std::string_view path)
{
    auto it = m_textures.find(path.data());
    if (it != m_textures.end()) { return it->second; }

    TextureData textureData = loadTextureFromFile(path);

    Format format = Format::RGBA;
    ByteFormat byteFormat = ByteFormat::RGBA;
    if (textureData.channels == 1)
    {
        format = Format::RED;
        byteFormat = ByteFormat::RED;
    }
    else if (textureData.channels == 3)
    {
        format = Format::RGB;
        byteFormat = ByteFormat::RGB;
    }

    Texture* newTexture = new Texture();
    id_t tex = m_gpurm->createTexture({
        .width = textureData.width,
        .height = textureData.height,
        .format = format,
        .byteFormat = byteFormat,
        .initialData = textureData.data,
    });
    newTexture->m_ID = tex;
    auto* texture = m_textures.insert(std::make_pair(path, newTexture)).first->second;
    //m_files.push_back(createAssetFile(path, FileView{ texture }));

    textureData.freeData();

    return texture;
}

Mesh* AssetManager::loadMesh(MeshType type, VertexIndexTuple& mesh)
{
    Mesh* newMesh = new Mesh();
    MeshHandles handles = m_gpurm->createMesh(MeshInfo<Vertex>{
        .vertices = mesh.vertices,
        .indices = mesh.indices,
        .amountAndOffset = {
            std::make_pair(3, offsetof(Vertex, position)),
            std::make_pair(3, offsetof(Vertex, normal)),
            std::make_pair(2, offsetof(Vertex, texcoord)),
        }
    });
    newMesh->m_meshType = type;
    newMesh->m_vao = handles.vao;
    newMesh->m_vbo = handles.vbo;
    newMesh->m_ebo = handles.ebo;
    newMesh->m_indices = mesh.indices;

    return newMesh;
}

Mesh* AssetManager::getMesh(MeshType type)
{
    auto it = m_nativeModels.find(type);
    if (it != m_nativeModels.end()) { return it->second; }


    VertexIndexTuple meshData = getNativeMeshByType(type);
    Mesh* mesh = loadMesh(type, meshData);
    auto* model = m_nativeModels.insert({type, mesh}).first->second;

    return model;
}

Mesh* AssetManager::getMesh(std::string_view name)
{
    auto it = m_importedModels.find(name.data());
    if (it != m_importedModels.end()) { return it->second; }

    // If imported model is not loaded, return a quad
    return getMesh(MeshType::Quad);
}

Model* AssetManager::getModel(std::string_view path)
{
    std::string strPath{ path };
    auto it = m_models.find(strPath);
    if (it != m_models.end()) { return it->second; }

    Model* newModel = new Model();
    ModelData* modelData = loadModel(path, false);

    for (auto mesh : *modelData)
    {
        Mesh* m = loadMesh(MeshType::Custom, mesh.vertexIndex);
        m_importedModels.insert({mesh.name, m});
        newModel->m_meshes.push_back({mesh.name, m});
    }

    auto* model = m_models.insert(std::make_pair(path, newModel)).first->second;
    //m_files.push_back(createAssetFile(path, FileView{ model }));
    return model;
}
/*
Shader &AssetManager::getShader(std::string_view path)
{
    std::string basePath{ path };
    if (m_shaders.find(basePath) != m_shaders.end()) { return m_shaders.at(basePath); }

    Shader newShader{};
    newShader.create(path);
    auto &shader = m_shaders.insert(std::make_pair(path, newShader)).first->second;
    m_files.push_back(createAssetFile(basePath + ".vert", FileView{ shader }));
    m_files.push_back(createAssetFile(basePath + ".frag", FileView{ shader }));
    return shader;
}
*/

void AssetManager::loadDefaultResources()
{
    getMesh(MeshType::Quad);
    getMesh(MeshType::Cube);
    getMesh(MeshType::Sphere);
    getMesh(MeshType::CubeMap);
    getMesh(MeshType::TriangleMap);
}

void AssetManager::checkFileModification()
{
    for (AssetFile &file : m_files) {
        try {
            auto lastWriteTime = std::filesystem::last_write_time(file.path);
            if (file.lastWriteTime != lastWriteTime) {
                logger::logInfo("Reloading file: {}", file.path);
                file.handle.onFileChanged(file.path);
                file.lastWriteTime = lastWriteTime;
            }
        } catch (std::filesystem::filesystem_error &e) {
            logger::logError("{}", e.what());
        }
    }
}
