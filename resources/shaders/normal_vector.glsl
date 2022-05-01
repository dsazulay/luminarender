#shader vertex
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

#shader fragment
#version 410

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}

#shader geometry
#version 410 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VARYINGS
{
    vec3 normal;
} Varyings[];

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
};

const float MAGNITUDE = 0.4;

void generateLine(int index)
{
    gl_Position = u_projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = u_projection * (gl_in[index].gl_Position + vec4(Varyings[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

vec4 getAveragePos()
{
    return (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
}

void main()
{
    vec4 pos = getAveragePos();
    gl_Position = u_projection * pos;
    EmitVertex();
    gl_Position = u_projection * (pos + vec4(Varyings[0].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}