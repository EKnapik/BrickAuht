
// Texture info
Texture2D gAlbedo			: register(t0);
Texture2D gNormal			: register(t1);
Texture2D gPosition			: register(t2);
SamplerState basicSampler	: register(s0);


struct PointLight
{
	float4 Color;
	float3 Position; // Intensity is stored within the position's alpha value
};

cbuffer externalData : register(b0)
{
	PointLight pointLight;
	float3 cameraPosition;
	float width;
	float height;
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
	// input.position is in pixel space meaning each pixel has a value and it's (0 - width)x(0 - height)
	// 0,0 being at upper left, and width,height at lower right (same as uv style)
	// float valX = input.position.x / 1280;
	// float valY = input.position.y / 720;
	float2 gUV = float2(input.position.x / width, input.position.y / height);

	input.normal = normalize(input.normal);
	float3 gWorldPos = gPosition.Sample(basicSampler, gUV).xyz;
	float3 normal = gNormal.Sample(basicSampler, gUV).xyz;
	float3 dirToLight = normalize(pointLight.Position.xyz - gWorldPos);
	float pointLightAmount = saturate(dot(normal, dirToLight));

	// specular
	float3 toCamera = normalize(cameraPosition - gWorldPos);
	float3 refl = reflect(-dirToLight, normal);
	float spec = pow(max(dot(refl, toCamera), 0), 200);

	float4 surfaceColor = gAlbedo.Sample(basicSampler, gUV);

	//return gPosition.Sample(basicSampler, gUV);

	return float4(((pointLight.Color.xyz * pointLightAmount * surfaceColor.xyz) +
		spec), surfaceColor.a);
	return float4(0, 0, 0, 0);
}