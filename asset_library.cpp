#include "asset_library.h"
#include "primitives.h"
#include "importer.h"

//AssetLibrary& AssetLibrary::instance()
//{
//    static AssetLibrary instance_;
//    return instance_;
//}

AssetLibrary::~AssetLibrary()
{
    for (auto kv : m_shaders)
    {
        delete kv.second;
    }

    for (auto kv : m_materials)
    {
        delete kv.second;
    }

    for (auto kv : m_meshes)
    {
        delete kv.second;
    }

    for (auto kv : m_models)
    {
        delete kv.second;
    }
}

Shader* AssetLibrary::loadShader(const char* name, const char* shaderPath)
{
    if (isShaderLoaded(name))
    {
        LOG_INFO("Shader already in library");
        return m_shaders[name];
    }

    m_shaders[name] = new Shader(shaderPath);

    return m_shaders[name];
}

Shader* AssetLibrary::getShader(const char* name) {
    if (isShaderLoaded(name))
        return m_shaders[name];

    LOG_WARN("There is no shader with this name in library");
    return nullptr;
}

bool AssetLibrary::isShaderLoaded(const char* name) {
    return (m_shaders.find(name) != m_shaders.end());
}

Material* AssetLibrary::createMaterial(const char* name, const char* shader)
{
    ASSERT(isShaderLoaded(shader), "Shader not loaded!");
    return createMaterial(name, getShader(shader));
}

Material* AssetLibrary::createMaterial(const char* name, Shader* shader)
{
    ASSERT(shader != nullptr, "Shader not loaded!");
    m_materials[name] = new Material(shader);
    return m_materials[name];
}

Material* AssetLibrary::getMaterial(const char* name)
{
    if (isMaterialCreated(name))
        return m_materials[name];

    LOG_WARN("There is no material with this name in library");
    return nullptr;
}

bool AssetLibrary::isMaterialCreated(const char* name)
{
    return (m_materials.find(name) != m_materials.end());
}

Model* AssetLibrary::loadModel(const char *name, const char *path)
{
    if (isModelLoaded(name))
    {
        LOG_INFO("model already loaded");
        return m_models[name];
    }

    m_models[name] = Importer::loadModel(path);
    for (auto mesh : m_models[name]->m_meshes)
    {
        m_meshes[mesh.first] = mesh.second;
    }

    return m_models[name];
}

Model* AssetLibrary::getModel(const char* name)
{
    if (isModelLoaded(name))
        return m_models[name];

    LOG_WARN("model not loaded");
    return nullptr;
}

bool AssetLibrary::isMeshLoaded(const char *name)
{
    return m_meshes.find(name) != m_meshes.end();
}

Mesh* AssetLibrary::loadMesh(AssetLibrary::BasicMesh basicModel)
{
    Mesh* m;
    const char* name;
    switch (basicModel)
    {
        case BasicMesh::Cube:
            m = new Mesh(Primitives::getCubePrimitives());
            name = "cube";
            break;
        case BasicMesh::CubeMapModel:
            m = new Mesh(Primitives::getCubeMapPrimitives());
            name = "cubeMap";
            break;
        case BasicMesh::Quad:
            m = new Mesh(Primitives::getQuadPrimitives());
            name = "quad";
            break;
        case BasicMesh::Sphere:
            m = new Mesh(Primitives::getSpherePrimitives());
            name = "sphere";
            break;
        default:
            LOG_WARN("No basic model found!");
            break;
    }
    m_meshes[name] = m;

    return m;
}

Mesh *AssetLibrary::getMesh(const char *name) {
    if (isMeshLoaded(name))
        return m_meshes[name];

    LOG_WARN("There is no material with this name in library");
    return nullptr;
}

bool AssetLibrary::isModelLoaded(const char* name)
{
    return m_models.find(name) != m_models.end();
}

Texture* AssetLibrary::load2DTexture(const char *name, const std::string &file, const std::string &directory)
{
    if (isTextureLoaded(name))
    {
        LOG_INFO("texture already loaded");
        return m_textures[name];
    }
    Texture* texture = Importer::loadTextureFromFile(file, directory);
    m_textures[name] = texture;
    return texture;
}

Texture* AssetLibrary::loadHDRTexture(const char *name, const std::string &file, const std::string &directory)
{
    if (isTextureLoaded(name))
    {
        LOG_INFO("texture already loaded");
        return m_textures[name];
    }
    Texture* texture = Importer::loadHDRTextureFromFile(file, directory);
    m_textures[name] = texture;
    return texture;
}

Texture *AssetLibrary::loadCubeMapTexture(const char *name, const std::vector<std::string> faces,
                                          const std::string &directory)
{
    if (isTextureLoaded(name))
    {
        LOG_INFO("texture already loaded");
        return m_textures[name];
    }
    Texture* texture = Importer::loadCubeMapFromFiles(faces, directory);
    m_textures[name] = texture;
    return texture;
}

Texture *AssetLibrary::getTexture(const char *name)
{
    if (isTextureLoaded(name))
        return m_textures[name];

    LOG_WARN("texture not loaded");
    return nullptr;
}

bool AssetLibrary::isTextureLoaded(const char *name)
{
    return m_textures.find(name) != m_textures.end();
}
