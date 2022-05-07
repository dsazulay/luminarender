#properties
Color u_color = 1,1,1,1
2D u_mainTex = white
#endproperties

#shader vertex
#version 410 core

uniform mat4 u_lightSpaceMatrix;
out vec4 v_fragPosLightSpace;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec3 v_normal;
out vec3 v_worldPos;
out vec2 v_uv;

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
};

uniform mat4 u_model;
uniform mat3 u_normalMatrix;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1);
    v_normal = u_normalMatrix * a_normal;
    v_worldPos = vec3(u_model * vec4(a_position, 1));
    v_uv = a_uv;

    v_fragPosLightSpace = u_lightSpaceMatrix * u_model * vec4(a_position, 1);
}

#shader fragment
#version 410 core

in vec3 v_normal;
in vec3 v_worldPos;
in vec2 v_uv;

out vec4 fragColor;

uniform vec4 u_color;
uniform sampler2D u_mainTex;

in vec4 v_fragPosLightSpace;
uniform sampler2D u_shadowMap;
uniform vec3 u_lightPos;

#include "lighting.glsl"

vec4 lambert(Light light, vec3 normal, vec3 color);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_worldPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
    shadow = 0.0;

    return shadow;
}

void main()
{
    fragColor = vec4(0, 0, 0, 0);
    vec3 normal = normalize(v_normal);
    vec4 mainTex = texture(u_mainTex, v_uv);

    float shadow = ShadowCalculation(v_fragPosLightSpace);
    int lightCount = getLightCount();
    for (int i = 0 ; i < lightCount; i++)
    {
        Light l = getLight(i, v_worldPos);
        fragColor += lambert(l, normal, mainTex.xyz * u_color.rgb) * (1 - shadow);
    }
}

vec4 lambert(Light light, vec3 normal, vec3 color)
{
    float NdotL = max(0.0, dot(normal, light.direction));
    vec3 radiance = color * light.color * NdotL * light.attenuation;

    return vec4(radiance, 1.0);
}
