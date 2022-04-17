#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec3 v_normal;
out vec3 v_worldPos;
out vec3 v_viewDir;
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
    v_viewDir = u_viewPos.xyz - v_worldPos;
    v_uv = a_uv;
}
