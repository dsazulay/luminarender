#properties
Color u_color = 1,1,1,1
2D u_mainTex = white
#endproperties

#shader vertex
#version 410 core

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
}

#shader fragment
#version 410 core

in vec3 v_normal;
in vec3 v_worldPos;
in vec2 v_uv;

out vec4 fragColor;

uniform vec4 u_color;
uniform sampler2D u_mainTex;

#include "lighting.glsl"

vec4 lambert(Light light, vec3 normal, vec3 color);

void main()
{
    fragColor = vec4(0, 0, 0, 0);
    vec3 normal = normalize(v_normal);
    vec4 mainTex = texture(u_mainTex, v_uv);

    int lightCount = getLightCount();
    for (int i = 0 ; i < lightCount; i++)
    {
        Light l = getLight(i, v_worldPos);
        fragColor += lambert(l, normal, mainTex.xyz * u_color.rgb);
    }
}

vec4 lambert(Light light, vec3 normal, vec3 color)
{
    float NdotL = max(0.0, dot(normal, light.direction));
    vec3 radiance = color * light.color * NdotL * light.attenuation;

    return vec4(radiance, 1.0);
}
