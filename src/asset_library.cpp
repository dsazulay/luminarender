#include "asset_library.h" 

#include "samples/sample_resources.h"
#include "importer.h"

#include "glm/vec4.hpp"

AssetLibrary& AssetLibrary::instance()
{
    static AssetLibrary instance_;
    return instance_;
}

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
    m_materials[name]->name(name);
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

Mesh* AssetLibrary::loadMesh(const char* name, BasicMeshType basicModel)
{
    if (isMeshLoaded(name))
    {
        LOG_WARN("mesh already loaded");
        return m_meshes[name];
    }

    Mesh* m = new Mesh(basicModel);
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

AssetLibrary::AssetLibrary()
{
    loadDefaultResources();
}

void AssetLibrary::loadDefaultResources()
{
    loadShader("skybox", SampleResources::shader_skybox);
    loadShader("unlit", SampleResources::shader_unlit);
    loadShader("lambert", SampleResources::shader_lambert);
    loadShader("pbr", SampleResources::shader_pbr);
    
    loadMesh("quad", BasicMeshType::Quad);
    loadMesh("cube", BasicMeshType::Cube);
    loadMesh("sphere", BasicMeshType::Sphere);
    loadMesh("cubeMap", BasicMeshType::CubeMap);
    loadMesh("triangleMap", BasicMeshType::TriangleMap);

    load2DTexture(DefaultResources::texWhite, texture_defaultWhite, texture_dir);
}

void AssetLibrary::createDefaultResources()
{
    Material* defaultPbr = createMaterial("defaultPbr", "pbr");
    defaultPbr->setProperty("u_albedo", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
    defaultPbr->setProperty("u_roughness", 0.8f);
}
