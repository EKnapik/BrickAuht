#include "GameEntity.h"

using namespace GMath;
GameEntity::GameEntity(Mesh * mesh, Material* material)
{
	// Set the mesh and add to its reference count
	this->mesh = mesh;
	mesh->AddReference();

	// Initialize the member variables.
	SetIdentity4X4(&world);
	ZeroVec3(&position);
	ZeroVec3(&rotation);
	SetVector3(&scale, 1, 1, 1);

	this->material = material;
	this->material->AddReference();
}

GameEntity::~GameEntity()
{
	mesh->Release();
	material->Release();
}

MAT4X4 * GameEntity::GetWorld()
{
#ifdef WITH_DX
	SetTransposeMatrix(&world, &(CreateScaleMatrix(&scale) * CreateRotationMatrix(&rotation) * CreateTranslationMatrix(&position)));
#else
	SetMatrx(&world, &(CreateScaleMatrix(&scale) * CreateRotationMatrix(&rotation) * CreateTranslationMatrix(&position)));
#endif
	return &world;
}

void GameEntity::SwapMesh(Mesh * newMesh)
{
	mesh->Release();
	mesh = newMesh;
	mesh->AddReference();
}

void GameEntity::PrepareShader(MAT4X4 * view, MAT4X4* projection)
{
	DirectionalLight light;
	light.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 0.0f);
	light.DiffuseColor = VEC4(0, 0, 1, 0);
	light.Direction = VEC3(1, -1, 0);
	PrepareShader(view, projection, &light);
}

void GameEntity::PrepareShader(MAT4X4 * view, MAT4X4 * projection, DirectionalLight * dLight)
{
	// Send data to shader variables
	material->GetVertexShader()->SetMatrix4x4("world", *GetWorld());
	material->GetVertexShader()->SetMatrix4x4("view", *view);
	material->GetVertexShader()->SetMatrix4x4("projection", *projection);
	material->GetVertexShader()->CopyAllBufferData();


	material->GetPixelShader()->SetSamplerState("basicSampler", material->GetSamplerState());
	material->GetPixelShader()->SetShaderResourceView("diffuseTexture", material->GetSRV());

	// Set the data for the light so that the shader can utilize it
	material->GetPixelShader()->SetData(
		"light",
		dLight,
		sizeof(DirectionalLight));

	// Create a ground light so every object is lit a little bit from the ground
	DirectionalLight gLight;
	gLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
	gLight.DiffuseColor = VEC4(71.0f/255.0f, 28.0f / 255.0f, 1.0f / 255.0f, 1.0f);
	gLight.Direction = VEC3(0, 1, 0);

	material->GetPixelShader()->SetData(
		"groundLight",
		&gLight,
		sizeof(DirectionalLight));

	PointLight pLight;
	pLight.Color = VEC4(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f, 1.0f);
	pLight.Position = VEC3(0, 5, 0);

	HRESULT result = material->GetPixelShader()->SetData(
		"pointLight",
		&pLight,
		sizeof(PointLight));

	material->GetPixelShader()->CopyAllBufferData();

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();
}

void GameEntity::Release()
{
	references--;
	if (references == 0)
	{
		delete this;
	}
}
