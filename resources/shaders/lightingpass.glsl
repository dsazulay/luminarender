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

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_albedospec;
#ifdef SHADOWS
    uniform sampler2D u_shadowMap;
#endif
uniform sampler2D u_ssao;
uniform sampler2D u_gposition;

#include "lighting.glsl"
#ifdef SHADOWS
    #include "shadows.glsl"
#endif

const float near = 0.1;
const float far = 100.0;

vec3 lambert(Light light, vec3 normal, vec3 color)
{
    float NdotL = max(0.0, dot(normal, light.direction));
    vec3 radiance = color * light.color * NdotL * light.attenuation;

    return radiance;
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
    vec4 mainTex = texture(u_albedospec, v_uv);

    int lightCount = getLightCount();
    for (int i = 0 ; i < lightCount; i++)
    {
        Light l = getLight(i, worldPos);
        fragColor.rgb += lambert(l, normal, mainTex.xyz);
    }

    #ifdef SHADOWS
        float shadow = 1 - shadowCalculation(lightSpaceMatrix * vec4(worldPos, 1), shadowLightPos.rgb, worldPos, normal);
        fragColor *= shadow;
    #endif

    float ao = texture(u_ssao, v_uv).r;
    vec3 ambient = vec3(0.3 * mainTex.rgb * ao);
    fragColor.rgb += ambient;
}
