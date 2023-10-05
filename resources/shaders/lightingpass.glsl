#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 v_uv;

void main()
{
    v_uv = a_uv;
    gl_Position = vec4(a_position * 2.0, 1.0);
}

#shader fragment
#version 410 core

#define SHADOWS

in vec2 v_uv;

out vec4 fragColor;

uniform sampler2D u_gposition;
uniform sampler2D u_normal;
uniform sampler2D u_albedospec;
#ifdef SHADOWS
    uniform sampler2D u_shadowMap;
#endif
uniform sampler2D u_ssao;

#include "lighting.glsl"
#ifdef SHADOWS
    #include "shadows.glsl"
#endif

vec3 lambert(Light light, vec3 normal, vec3 color)
{
    float NdotL = max(0.0, dot(normal, light.direction));
    vec3 radiance = color * light.color * NdotL * light.attenuation;

    return radiance;
}

void main()
{
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 worldPos = texture(u_gposition, v_uv).rgb;
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
