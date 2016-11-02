
// Texture info
Texture2D gAlbedo			: register(t0);
Texture2D gNormal			: register(t1);
Texture2D gPosition			: register(t2);
SamplerState basicSampler	: register(s0);


struct PointLight
{
	float4 Color;
	float3 Position; // Intensity is stored within the position's alpha value
	float Radius;
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

	float3 gWorldPos = gPosition.Sample(basicSampler, gUV).xyz;
	// need to unpack normal
	float3 normal = (gNormal.Sample(basicSampler, gUV).xyz * 2.0f) - 1.0f;
	float4 surfaceColor = gAlbedo.Sample(basicSampler, gUV);

	// Check for clip
	// CLIPPING MIGHT NOT BE THE BEST CHOICE BUT IT IS A CHOICE
	clip(surfaceColor.x + surfaceColor.y + surfaceColor.z + -0.01);

	float3 dirToLight = normalize(pointLight.Position.xyz - gWorldPos);
	float pointLightAmount = saturate(dot(normal, dirToLight));
	// Light Attenuation f = 1 / ((d/r) + 1)^2
	float dist = length(pointLight.Position.xyz - gWorldPos);
	pointLightAmount = pointLightAmount / pow(((dist / pointLight.Radius) + 1), 2);

	// specular
	float3 toCamera = normalize(cameraPosition - gWorldPos);
	float3 refl = reflect(-dirToLight, normal);
	float spec = pow(max(dot(refl, toCamera), 0), 200);


	//return gPosition.Sample(basicSampler, gUV);

	return float4(((pointLight.Color.xyz * pointLightAmount * surfaceColor.xyz) +
		spec), surfaceColor.a);
	return float4(0, 0, 0, 0);
}