#pragma once

#include "shader.h"
#include "material.h"

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

    void loadDefaultResources();
    void createDefaultResources();

    Shader* getShader(const char* name);
    Material* getMaterial(const char* name);

private:
    AssetLibrary();
    void generateSSAONoiseTexture();

    bool isShaderLoaded(const char* name);
    bool isMaterialCreated(const char* name);

    std::unordered_map<std::string, Shader*> m_shaders;
    std::unordered_map<std::string, Material*> m_materials;

    static constexpr char* texture_defaultWhite = (char*) "default_white.png";
    static constexpr char* texture_dir = (char*) "resources/textures";
};
