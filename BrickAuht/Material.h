#pragma once

#include "SimpleShader.h"


class Material {
public:
	Material(ID3D11ShaderResourceView* SRV, ID3D11SamplerState* SamplerState);

	void AddReference() { references++; };

	void Release();

	bool transparency = false;

	ID3D11ShaderResourceView* GetSRV() { return SRV; };
	ID3D11SamplerState* GetSamplerState() { return SamplerState; }
private:
	int references = 0;

	ID3D11ShaderResourceView* SRV;
	ID3D11SamplerState* SamplerState;

	~Material();
	
};