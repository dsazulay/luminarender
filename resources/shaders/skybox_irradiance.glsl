#shader vertex
#version 410 core
layout (location = 0) in vec3 a_pos;

out vec3 v_texcoords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_texcoords = a_pos;
    gl_Position =  u_projection * u_view * vec4(a_pos, 1.0);
}

#shader fragment
#version 410 core
in vec3 v_texcoords;

out vec4 fragColor;

uniform samplerCube u_environmentMap;

const float PI = 3.14159265359;

void main()
{
    vec3 N = normalize(v_texcoords);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(u_environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    fragColor = vec4(irradiance, 1.0);
}
