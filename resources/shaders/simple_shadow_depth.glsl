#shader vertex
#version 410 core

layout (location = 0) in vec3 a_pos;

uniform mat4 u_model;

#include "lighting.glsl"

void main()
{
    gl_Position = lightSpaceMatrix * u_model * vec4(a_pos, 1.0);
}

#shader fragment
#version 410 core

void main()
{

}

