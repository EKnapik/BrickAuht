
// Texture info
Texture2D gAlbedo			: register(t0);
Texture2D gNormal			: register(t1);
Texture2D gPosition			: register(t2);
SamplerState basicSampler	: register(s0);


struct PointLight
{
	float4 Color;
	float4 Position; // Intensity is stored within the position's alpha value
};

cbuffer externalData : register(b0)
{
	PointLight pointLight;
	float3 cameraPosition;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};


// --------------------------------------------------------
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 gWorldPos = gPosition.Sample(basicSampler, input.position.xy);
	float3 gNormal = float3(0, 0, 0); // has the same issue as world pos
	float3 dirToLight = normalize(pointLight.Position.xyz - gWorldPos);
	float pointLightAmount = saturate(dot(gNormal, dirToLight));

	// specular
	float3 toCamera = normalize(cameraPosition - gWorldPos);
	float3 refl = reflect(-dirToLight, gNormal);
	float spec = pow(max(dot(refl, toCamera), 0), 200);

	// also that sampler problem where is the propper sample to be done?
	float4 surfaceColor = gAlbedo.Sample(basicSampler, input.uv); 

	// TODO:
	// I fear that there will be a sampling issue because what is the propper
	// place to sample the texture and where do I get that info??
	// Desirebly I was the sphere as if it were flattened onto the screen
	return gPosition.Sample(basicSampler, input.uv);

	return float4(((pointLight.Color.xyz * pointLightAmount * surfaceColor.xyz) +
		spec), surfaceColor.a);
	return float4(0, 0, 0, 0);
}