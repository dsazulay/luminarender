#include "asset_library.h" 

#include "../sample_resources.h"
#include "../log.h"

#include <glm/vec4.hpp>

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

    LOG_WARN("There is no material with \"{}\" name in library", name);
    return nullptr;
}

bool AssetLibrary::isMaterialCreated(const char* name)
{
    return (m_materials.find(name) != m_materials.end());
}

AssetLibrary::AssetLibrary()
{
}

void AssetLibrary::loadDefaultResources()
{
    loadShader("skybox", SampleResources::shader_skybox);
    loadShader("skytriangle", SampleResources::shader_skytriangle);
    loadShader("unlit", SampleResources::shader_unlit);
    loadShader("lambert", SampleResources::shader_lambert);
    loadShader("pbr", SampleResources::shader_pbr);


    generateSSAONoiseTexture();
}

void AssetLibrary::createDefaultResources()
{
    Material* defaultPbr = createMaterial("defaultPbr", "pbr");
    defaultPbr->setProperty("u_albedo", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
    defaultPbr->setProperty("u_roughness", 0.8f);
}

void AssetLibrary::generateSSAONoiseTexture()
{
    // TODO: implement
}
