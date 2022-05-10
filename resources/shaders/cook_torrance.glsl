#properties
Color u_albedo = 1,1,1,1
Float u_metallic = 1.0
Float u_roughness = 1.0
2D u_albedoTex = white
2D u_metallicTex = white
2D u_roughnessTex = white
2D u_aoTex = white
#endproperties

#shader vertex
#version 410 core

#define SHADOWS

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out Varyings
{
    vec3 normal;
    vec3 worldPos;
    vec3 viewDir;
    vec2 uv;
    #ifdef SHADOWS
        vec4 fragPosLightSpace;
    #endif
} v_out;

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
};

uniform mat4 u_model;
uniform mat3 u_normalMatrix;
#ifdef SHADOWS
    uniform mat4 u_lightSpaceMatrix;
#endif

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1);
    v_out.normal = u_normalMatrix * a_normal;
    v_out.worldPos = vec3(u_model * vec4(a_position, 1));
    v_out.viewDir = u_viewPos.xyz - v_out.worldPos;
    v_out.uv = a_uv;
    #ifdef SHADOWS
        v_out.fragPosLightSpace = u_lightSpaceMatrix * u_model * vec4(a_position, 1);
    #endif
}

#shader fragment
#version 410 core

#define SHADOWS

in Varyings
{
    vec3 normal;
    vec3 worldPos;
    vec3 viewDir;
    vec2 uv;
    #ifdef SHADOWS
        vec4 fragPosLightSpace;
    #endif
} v_in;

out vec4 fragColor;

const float PI = 3.14159265359;

uniform vec4 u_albedo;
uniform float u_metallic;
uniform float u_roughness;

uniform sampler2D u_albedoTex;
uniform sampler2D u_metallicTex;
uniform sampler2D u_roughnessTex;
uniform sampler2D u_aoTex;

uniform samplerCube u_irradianceTex;
uniform samplerCube u_prefilterMap;
uniform sampler2D u_brdfLUT;

#ifdef SHADOWS
    uniform sampler2D u_shadowMap;
    uniform vec3 u_lightPos;
#endif

#include "lighting.glsl"
#ifdef SHADOWS
    #include "shadows.glsl"
#endif

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 f0, float roughness)
{
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0 - cosTheta, 5.0);
}

float distributionGGX(vec3 normal, vec3 halfway, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, halfway), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}

float geometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 cookTorrance(vec3 albedo, vec3 lightDir, vec3 normal, vec3 viewDir, vec3 f0, float roughness, float metallic)
{
    vec3 halfway = normalize(lightDir + viewDir);

    float N = distributionGGX(normal, halfway, roughness);
    float G = geometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(max(dot(halfway, viewDir), 0.0), f0);

    vec3 num = N * G * F;
    float denom = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);
    vec3 specular = num / max(denom, 0.001);

    vec3 kD = (1 - metallic) * (1 - F);

    return kD * albedo / PI + specular;
}

void main()
{
    vec3 albedo = pow(texture(u_albedoTex, v_in.uv).rgb, vec3(2.2)) * u_albedo.rgb;
    float metallic = texture(u_metallicTex, v_in.uv).r * u_metallic;
    float roughness = texture(u_roughnessTex, v_in.uv).r * u_roughness;

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    fragColor = vec4(0, 0, 0, 1);
    vec3 normal = normalize(v_in.normal);
    vec3 viewDir = normalize(v_in.viewDir);
    vec3 reflection = reflect(-viewDir, normal);

    for (int i = 0; i < getLightCount(); i++)
    {
        Light l = getLight(i, v_in.worldPos);
        vec3 radiance = cookTorrance(albedo, l.direction, normal, viewDir, f0, roughness, metallic);

        float NdotL = max(dot(normal, l.direction), 0.0);
        fragColor += vec4(radiance * l.color * l.attenuation * NdotL, 0.0);
    }

    #ifdef SHADOWS
        float shadow = 1 - shadowCalculation(v_in.fragPosLightSpace);
        fragColor *= shadow;
    #endif

    // global illumination
    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), f0, roughness);
    vec3 kD = (1 - metallic) * (1 - F);

    vec3 irradiance = texture(u_irradianceTex, normal).rgb;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_prefilterMap, reflection,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * irradiance * albedo + specular) * texture(u_aoTex, v_in.uv).r;

    fragColor += vec4(ambient, 0.0);

    // tone mapping and linear workspace conversion
    fragColor = fragColor / (fragColor + 1);
    fragColor = pow(fragColor, vec4(1/2.2));
}

