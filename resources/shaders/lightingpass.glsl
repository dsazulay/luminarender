#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 v_uv;
out vec3 v_viewRay;

const float aspectRatio = 999.0 / 676.0;
const float tanHalfFov = tan(radians(45.0 * 0.5));

void main()
{
    v_uv = a_uv;
    vec4 posCS = vec4(a_position * 2.0, 1.0);
    v_viewRay.x = posCS.x * aspectRatio * tanHalfFov;
    v_viewRay.y = posCS.y * tanHalfFov;
    v_viewRay.z = -1.0;
    gl_Position = posCS;
}

#shader fragment
#version 410 core

#define SHADOWS

in vec2 v_uv;
in vec3 v_viewRay;

out vec4 fragColor;

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
    vec4 u_camForward;
};

const float PI = 3.14159265359;

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;
#ifdef SHADOWS
    uniform sampler2D u_shadowMap;
#endif
uniform sampler2D u_ssao;
uniform sampler2D u_orm;

uniform samplerCube u_irradianceTex;
uniform samplerCube u_prefilterMap;
uniform sampler2D u_brdfLUT;

#include "lighting.glsl"
#ifdef SHADOWS
    #include "shadows.glsl"
#endif

const float near = 0.1;
const float far = 100.0;


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



float sampleLinearDepth(vec2 uv)
{
    float depth = texture(u_depth, uv).r * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

void main()
{
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    float depth = sampleLinearDepth(v_uv);
    vec3 viewRay = v_viewRay * depth;
    vec3 worldPos = vec3(inverse(u_view) * vec4(viewRay, 1.0));
    vec3 normal = texture(u_normal, v_uv).rgb;

    vec3 albedo = pow(texture(u_albedo, v_uv).rgb, vec3(2.2));
    vec3 orm = texture(u_orm, v_uv).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 viewDir = normalize(u_viewPos.xyz - worldPos);
    vec3 reflection = reflect(-viewDir, normal);

    for (int i = 0; i < getLightCount(); i++)
    {
        Light l = getLight(i, worldPos);
        vec3 radiance = cookTorrance(albedo, l.direction, normal, viewDir, f0, roughness, metallic);

        float NdotL = max(dot(normal, l.direction), 0.0);
        fragColor += vec4(radiance * l.color * l.attenuation * NdotL, 0.0);
    }

    #ifdef SHADOWS
        float shadow = 1 - shadowCalculation(lightSpaceMatrix * vec4(worldPos, 1), shadowLightPos.rgb, worldPos, normal);
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

    float ssao = texture(u_ssao, v_uv).r;
    vec3 ambient = (kD * irradiance * albedo + specular) * ssao * ao;

    fragColor += vec4(ambient, 0.0);

    // tone mapping and linear workspace conversion
    fragColor = fragColor / (fragColor + 1);
    fragColor = pow(fragColor, vec4(1/2.2));
}
