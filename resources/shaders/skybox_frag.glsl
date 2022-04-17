#version 410 core
in vec3 v_texcoords;

out vec4 fragColor;

uniform samplerCube u_mainTex;

void main()
{
    vec3 envColor = texture(u_mainTex, v_texcoords).rgb;

    envColor = envColor / (envColor + vec3(1.0));
//    envColor = pow(envColor, vec3(1.0/2.2));

    fragColor = vec4(envColor, 1.0);
}
