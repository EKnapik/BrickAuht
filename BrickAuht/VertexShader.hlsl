
// Constant Buffer
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;

	matrix shadowView;
	matrix shadowProjection;
};


struct VertexShaderInput
{ 
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};


struct VertexToPixel
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
	float4 posForShadow	: TEXCOORD1;
};

// --------------------------------------------------------
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// The vertex's position (input.position) must be converted to world space,
	// then camera space (relative to our 3D camera), then to proper homogenous 
	// screen-space coordinates.  This is taken care of by our world, view and
	// projection matrices.  
	//
	// First we multiply them together to get a single matrix which represents
	// all of those transformations (world to view to projection space)
	matrix worldViewProj = mul(mul(world, view), projection);

	// Then we convert our 3-component position vector to a 4-component vector
	// and multiply it by our final 4x4 matrix.
	//
	// The result is essentially the position (XY) of the vertex on our 2D 
	// screen and the distance (Z) from the camera (the "depth" of the pixel)
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	// Calculate the world position of the vert
	output.worldPos = mul(float4(input.position, 1), world).xyz;

	// Set the normal
	output.normal = mul(input.normal, (float3x3)world);

	// Copy over the inputs UV coordinates
	output.uv = input.uv;

	matrix shadowWVP = mul(mul(world, shadowView), shadowProjection);
	output.posForShadow = mul(float4(input.position, 1), shadowWVP);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}