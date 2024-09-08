#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 v_uv;
out vec3 v_viewRay;

const float aspectRatio = 956.0 / 676.0;
const float tanHalfFov = tan(radians(45.0 * 0.5));

void main()
{
    v_uv = a_uv;
    vec4 posCS = vec4(a_position * 2.0, 1.0);
    v_viewRay.x = posCS.x * aspectRatio * tanHalfFov;
    v_viewRay.y = posCS.y * tanHalfFov;
    v_viewRay.z = -1.0;
    gl_Position = posCS;

}

#shader fragment
#version 410 core

in vec2 v_uv;
in vec3 v_viewRay;

out float fragColor;

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
    vec4 u_camForward;
};

uniform sampler2D u_depth;
uniform sampler2D u_normal;
uniform sampler2D u_noise;
uniform vec3 u_samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

const float near = 0.1;
const float far = 100.0;

float sampleLinearDepth(vec2 uv)
{
    float depth = texture(u_depth, uv).r * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos = v_viewRay * sampleLinearDepth(v_uv);
    vec3 normal = transpose(inverse(mat3(u_view))) * texture(u_normal, v_uv).rgb;
    normal = normalize(normal);
    vec3 randomVec = normalize(texture(u_noise, v_uv * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * u_samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = u_projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = -sampleLinearDepth(offset.xy); // get depth value of kernel sample
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    fragColor = occlusion;
}
