#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 1.0;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(800.0f/4.0f, 600.0f/4.0f); 

uniform mat4 projection;

void main()
{
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));  // 这样一定会得到平行UV的单位向量，虽然以我们的数据点乘结果应该是0，即减号后面的结果是0
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 sample = TBN * samples[i]; // From tangent to view-space
        sample = fragPos + sample * radius;   // 将变换后的采样方向向量缩放radius（采样半径），并从当前片段位置出发，得到相机空间中的采样点位置。
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide, from clip-space to NDC
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0, from NDC to screen-space
        
        // get sample depth
        // 这里转换到相机空间，z轴和右手系z轴相反，需要取负值
        float sampleDepth = -texture(gPositionDepth, offset.xy).w; // Get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth ));
        occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = occlusion / kernelSize;
    
    FragColor = 1.0 - occlusion;
}