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


void GameEntity::Release()
{
	references--;
	if (references == 0)
	{
		delete this;
	}
}
