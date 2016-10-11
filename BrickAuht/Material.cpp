#include "Material.h"

Material::Material(SimpleVertexShader * vertex, SimplePixelShader * pixel, ID3D11ShaderResourceView* SRV, ID3D11SamplerState* SamplerState)
{
	vertexShader = vertex;
	pixelShader = pixel;
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
	delete vertexShader;
	delete pixelShader;
	SRV->Release();
	SamplerState->Release();
}
