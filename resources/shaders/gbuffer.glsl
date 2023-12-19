#properties
Color u_albedo = 1,1,1,1
Float u_metallic = 0.0
Float u_roughness = 1.0
2D u_albedoTex = white
2D u_metallicTex = white
2D u_roughnessTex = white
2D u_aoTex = white
#endproperties

#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out Varyings
{
    vec3 normal;
    vec3 worldPos;
    vec2 uv;
} v_out;


layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
    vec4 u_camForward;
};

uniform mat4 u_model;
uniform mat3 u_normalMatrix;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1);
    v_out.normal = u_normalMatrix * a_normal;
    v_out.worldPos = vec3(u_model * vec4(a_position, 1));
    v_out.uv = a_uv;
}

#shader fragment
#version 410 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo;

in Varyings
{
    vec3 normal;
    vec3 worldPos;
    vec2 uv;
} v_in;

uniform vec4 u_albedo;
uniform float u_metallic;
uniform float u_roughness;

uniform sampler2D u_albedoTex;
uniform sampler2D u_metallicTex;
uniform sampler2D u_roughnessTex;
uniform sampler2D u_aoTex;

void main()
{
    vec4 albedo = texture(u_albedoTex, v_in.uv) * u_albedo;
    float ao = texture(u_aoTex, v_in.uv).r;
    float roughness = texture(u_roughnessTex, v_in.uv).r * u_roughness;
    float metallic = texture(u_metallicTex, v_in.uv).r * u_metallic;
    g_albedo = albedo;
    g_normal = normalize(v_in.normal);
    g_position = vec3(ao, roughness, metallic);
}
