#properties
Color u_color = 1,1,1,1
2D u_mainTex = white
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
    vec2 uv;
    #ifdef SHADOWS
        vec4 fragPosLightSpace;
    #endif
} v_in;

out vec4 fragColor;

uniform vec4 u_color;
uniform sampler2D u_mainTex;
#ifdef SHADOWS
    uniform sampler2D u_shadowMap;
    uniform vec3 u_lightPos;
#endif

#include "lighting.glsl"
#ifdef SHADOWS
    #include "shadows.glsl"
#endif

vec4 lambert(Light light, vec3 normal, vec3 color);

void main()
{
    fragColor = vec4(0, 0, 0, 0);
    vec3 normal = normalize(v_in.normal);
    vec4 mainTex = texture(u_mainTex, v_in.uv);

    int lightCount = getLightCount();
    for (int i = 0 ; i < lightCount; i++)
    {
        Light l = getLight(i, v_in.worldPos);
        fragColor += lambert(l, normal, mainTex.xyz * u_color.rgb);
    }
    #ifdef SHADOWS
        float shadow = 1 - shadowCalculation(v_in.fragPosLightSpace);
        fragColor *= shadow;
    #endif
}

vec4 lambert(Light light, vec3 normal, vec3 color)
{
    float NdotL = max(0.0, dot(normal, light.direction));
    vec3 radiance = color * light.color * NdotL * light.attenuation;

    return vec4(radiance, 1.0);
}
