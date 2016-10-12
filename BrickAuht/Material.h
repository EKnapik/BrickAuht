#pragma once

#include "SimpleShader.h"


class Material {
public:
	Material(SimpleVertexShader* vertex, SimplePixelShader* pixel, ID3D11ShaderResourceView* SRV, ID3D11SamplerState* SamplerState);

	void AddReference() { references++; };

	void Release();

	SimpleVertexShader* GetVertexShader() { return vertexShader; }
	SimplePixelShader* GetPixelShader() { return pixelShader; }
	ID3D11ShaderResourceView* GetSRV() { return SRV; };
	ID3D11SamplerState* GetSamplerState() { return SamplerState; }
private:
	int references = 0;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	ID3D11ShaderResourceView* SRV;
	ID3D11SamplerState* SamplerState;

	~Material();
	
};