#version 410 core

in vec3 v_normal;
in vec3 v_worldPos;
in vec3 v_viewDir;
in vec2 v_uv;

out vec4 fragColor;

const float PI = 3.14159265359;

uniform vec4 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;

uniform samplerCube u_irradianceTex;
uniform samplerCube u_prefilterMap;
uniform sampler2D u_brdfLUT;

#include "lighting.glsl"

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
    vec3 f0 = vec3(0.04);
    f0 = mix(f0, u_albedo.rgb, u_metallic);

    fragColor = vec4(0, 0, 0, 1);
    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(v_viewDir);
    vec3 reflection = reflect(-viewDir, normal);

    for (int i = 0; i < getLightCount(); i++)
    {
        Light l = getLight(i, v_worldPos);
        vec3 radiance = cookTorrance(u_albedo.rgb, l.direction, normal, viewDir, f0, u_roughness, u_metallic);

        float NdotL = max(dot(normal, l.direction), 0.0);
        fragColor += vec4(radiance * l.color * l.attenuation * NdotL, 0.0);
    }

    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), f0, u_roughness);
    vec3 kD = (1 - u_metallic) * (1 - F);

    vec3 irradiance = texture(u_irradianceTex, normal).rgb;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_prefilterMap, reflection,  u_roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(normal, viewDir), 0.0), u_roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * irradiance * u_albedo.rgb + specular) * u_ao;

    fragColor += vec4(ambient, 0.0);

    fragColor = fragColor / (fragColor + 1);
    fragColor = pow(fragColor, vec4(1/2.2));
}
