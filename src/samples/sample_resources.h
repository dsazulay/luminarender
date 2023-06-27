#pragma once

#include <glm/glm.hpp>

class SampleResources
{
public:
    static constexpr char* shader_unlit = (char*) "resources/shaders/unlit.glsl";
    static constexpr char* shader_lambert = (char*) "resources/shaders/lambert.glsl";
    static constexpr char* shader_pbr = (char*) "resources/shaders/cook_torrance.glsl";
    static constexpr char* shader_skybox = (char*) "resources/shaders/skybox.glsl";
    static constexpr char* shader_skytriangle = (char*) "resources/shaders/skytriangle.glsl";
    static constexpr char* shader_skybox_equirectangular = (char*) "resources/shaders/skybox_equirectangular.glsl";
    static constexpr char* shader_skybox_irradiance = (char*) "resources/shaders/skybox_irradiance.glsl";
    static constexpr char* shader_prefilter = (char*) "resources/shaders/prefilter.glsl";
    static constexpr char* shader_brdf = (char*) "resources/shaders/brdf.glsl";

    static constexpr char* texture_dir = (char*) "resources/textures";
    static constexpr char* texture_skybox_dir = (char*) "resources/textures/skybox";
    static constexpr char* texture_spitfire_dir = (char*) "resources/textures/spitfire";
    static constexpr char* texture_metalbox_dir = (char*) "resources/textures/metalbox";
    static constexpr char* texture_cerberus_dir = (char*) "resources/textures/cerberus";
    static constexpr char* texture_container = (char*) "container.jpg";
    static constexpr char* texture_woodBox = (char*) "wood_box.png";
    static constexpr char* texture_spitfire = (char*) "spitfire_d.png";
    static constexpr char* texture_spitfire_metallic = (char*) "spitfire_m.png";
    static constexpr char* texture_spitfire_roughness = (char*) "spitfire_r.png";
    static constexpr char* texture_spitfire_ao = (char*) "spitfire_ao.png";
    static constexpr char* texture_spitfire_normal = (char*) "spitfire_n.png";
    static constexpr char* texture_metalbox = (char*) "metalbox_a.png";
    static constexpr char* texture_metalbox_roughness = (char*) "metalbox_r.png";
    static constexpr char* texture_metalbox_ao = (char*) "metalbox_ao.png";
    static constexpr char* texture_metalbox_normal = (char*) "metalbox_normal.png";
    static constexpr char* texture_cerberus = (char*) "cerberus_a.png";
    static constexpr char* texture_cerberus_metallic = (char*) "cerberus_m.png";
    static constexpr char* texture_cerberus_roughness = (char*) "cerberus_r.png";


    static constexpr char* texture_tiber = (char*) "tiber_2.hdr";
    static constexpr char* texture_green_park = (char*) "green_point_park.hdr";
    static constexpr char* texture_tank_farm = (char*) "tank_farm.hdr";


    static constexpr char* faces[] =
    {
        (char*) "right.jpg", (char*) "left.jpg", (char*) "top.jpg",
        (char*) "bottom.jpg", (char*) "front.jpg", (char*) "back.jpg"
    };

    static constexpr char* model_spitfire = (char*) "resources/models/spitfire.FBX";
    static constexpr char* model_cerberus = (char*) "resources/models/cerberus.FBX";

    static constexpr glm::vec3 object_positions[10] =
    {
        glm::vec3( 0.0f,  -5.0f,  -4.0f),
        glm::vec3( -1.5f,  -4.5f, -6.5f),
        glm::vec3(2.0f, -4.5f, -4.0f),
        glm::vec3(-2.8f, -3.0f, -4.3f),
        glm::vec3( 2.4f, -6.5f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
};
