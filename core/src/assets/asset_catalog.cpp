#include "asset_catalog.h"

#include "../logger.h"
#include "../log.h"
#include "model.h"
#include "primitives.h"

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
/*
Texture &AssetCatalog::getTexture(std::string_view path)
{
    std::string strPath{ path };
    if (m_textures.find(strPath) != m_textures.end()) { return m_textures.at(strPath); }

    Texture newTexture;
    newTexture.create(path);
    auto &texture = m_textures.insert(std::make_pair(path, newTexture)).first->second;
    m_files.push_back(createAssetFile(path, FileView{ texture }));
    return texture;
}
*/
Mesh* AssetCatalog::getModel(MeshType type)
{
    if (m_nativeModels.find(type) != m_nativeModels.end()) { return m_nativeModels.at(type); }

    Mesh* newModel = new Mesh();
    VertexIndexTuple mesh = getNativeMeshByType(type);
    MeshHandles handles = m_gpurm.createMesh(MeshInfo<Vertex>{
        .vertices = mesh.vertices,
        .indices = mesh.indices,
        .amountAndOffset = {
            std::make_pair(3, offsetof(Vertex, position)),
            std::make_pair(3, offsetof(Vertex, normal)),
            std::make_pair(2, offsetof(Vertex, texcoord)),
        }
    });
    newModel->m_meshType = type;
    newModel->m_vao = handles.vao;
    newModel->m_vbo = handles.vbo;
    newModel->m_ebo = handles.ebo;
    newModel->m_indices = mesh.indices;
    auto* model = m_nativeModels.insert(std::make_pair(type, newModel)).first->second;
    return model;
}
/*
Model &AssetCatalog::getModel(std::string_view path)
{
    std::string strPath{ path };
    if (m_models.find(strPath) != m_models.end()) { return m_models.at(strPath); }

    Model newModel;
    newModel.create(path);
    auto &model = m_models.insert(std::make_pair(path, newModel)).first->second;
    m_files.push_back(createAssetFile(path, FileView{ model }));
    return model;
}

Shader &AssetCatalog::getShader(std::string_view path)
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

void AssetCatalog::loadDefaultResources()
{
    getModel(MeshType::Quad);
    getModel(MeshType::Cube);
    getModel(MeshType::Sphere);
    getModel(MeshType::CubeMap);
    getModel(MeshType::TriangleMap);
}

void AssetCatalog::checkFileModification()
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
