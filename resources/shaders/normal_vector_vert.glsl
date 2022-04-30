#version 410 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out VARYINGS
{
    vec3 normal;
} Varyings;

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
    gl_Position = u_view * u_model * vec4(a_pos, 1.0);
    Varyings.normal = normalize(vec3(vec4(u_normalMatrix * a_normal, 0.0)));
}