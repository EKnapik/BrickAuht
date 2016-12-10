
cbuffer Data : register(b0)
{
	float2 dir; // the width of the screen
}


// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and such
Texture2D Pixels		: register(t0);
SamplerState Sampler	: register(s0);

// gaussian weights
const float weight[] = {
	0.2270270270, 0.1945945946, 0.1216216216,
	0.0540540541, 0.0162162162
};


// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 color = Pixels.Sample(Sampler, input.uv) * weight[0];
	
	for (int i = 1; i < 5; i++)
	{
		color += Pixels.Sample(Sampler, (input.uv + (dir*i))) * weight[i];
		color += Pixels.Sample(Sampler, (input.uv - (dir*i))) * weight[i];
	}

	return color;
}