// Texture info
Texture2D texNoise			: register(t0);
Texture2D gNormal			: register(t1);
Texture2D gPosition			: register(t2);
SamplerState basicSampler	: register(s0);

cbuffer externalData : register(b0)
{
	float3 samples[64];
	matrix view;
	matrix projection;
	float width;
	float height;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};


// parameters 
static int kernelSize = 64;
static float radius = 0.5;
static float bias = 0.025;

float main(VertexToPixel input) : SV_TARGET
{
	// tile noise texture over screen based on screen dimensions divided by noise size
	const float2 noiseScale = float2(width / 100, height / 100);

    // Get input for SSAO algorithm
    float3 fragPos = mul(float4(gPosition.Sample(basicSampler, input.uv).xyz, 1.0), view).xyz;
	float3 normal =  mul(float4(gNormal.Sample(basicSampler, input.uv).rgb * 2.0f - 1.0f, 1.0), view).xyz;
	float3 randomVec = normalize(texNoise.Sample(basicSampler, input.uv * noiseScale).xyz);
    // Create TBN change-of-basis matrix: from tangent-space to view-space
	float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
		float3 samplePos = mul(samples[i], TBN); // From tangent to view-space
		samplePos = fragPos + samplePos * radius;
        
        // project sample position (to sample texture) (to get position on screen/texture)
		float4 offset = float4(samplePos, 1.0);
        offset = mul(offset, projection); // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = mul(float4(gPosition.Sample(basicSampler, offset.xy).xyz, 1.0), view).z; // Get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
	//return 0.0;
    return occlusion;
}