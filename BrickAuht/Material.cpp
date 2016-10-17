#include "Material.h"

Material::Material(ID3D11ShaderResourceView* SRV, ID3D11SamplerState* SamplerState)
{
	this->SRV = SRV;
	this->SamplerState = SamplerState;
}

void Material::Release()
{
	references--;
	if (references <= 0)
	{
		delete this;
	}
}

Material::~Material()
{
	SRV->Release();
	SamplerState->Release();
}
