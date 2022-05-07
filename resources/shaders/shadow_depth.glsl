#shader vertex
#version 410 core

layout (location = 0) in vec3 a_pos;

uniform mat4 u_model;

void main()
{
    gl_Position = u_model * vec4(a_pos, 1.0);
}

#shader fragment
#version 410 core

void main()
{

}

#shader geometry
#version 410 core

layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[8];
};

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}