#pragma once
#include "GameMath.h"
#include "SimpleShader.h"
#include "Camera.h"

struct ParticleVertex
{
	int Type;
	float Age;
	VEC3 StartPosition;
	VEC3 StartVelocity;
	VEC4 StartColor;
	VEC4 MidColor;
	VEC4 EndColor;
	VEC3 StartMidEndSize;
};

class ParticleEmitter {
public:
	ParticleEmitter(
		ID3D11Device* device,
		SimpleVertexShader* particleVS,
		SimplePixelShader* particlePS,
		SimpleGeometryShader* particleGS,
		SimpleVertexShader* spawnVS,
		SimpleGeometryShader* spawnGS,
		ID3D11ShaderResourceView* texture,
		ID3D11SamplerState* sampler,
		ID3D11BlendState* particleBlendState,
		ID3D11DepthStencilState* particleDepthState);

	~ParticleEmitter();

	void Draw(ID3D11DeviceContext* context, Camera* camera, float deltaTime, float totalTime);

private:
	void SwapSOBuffers();
	void DrawSpawn(ID3D11DeviceContext* context, float deltaTime, float totalTime);


	// Particle-related
	ID3D11Buffer* particleVB;
	ID3D11Buffer* soBufferRead;
	ID3D11Buffer* soBufferWrite;
	bool spawnFlip;
	int frameCount;

	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	SimpleGeometryShader* particleGS;

	SimpleVertexShader* spawnVS;
	SimpleGeometryShader* spawnGS;

	ID3D11Texture1D* randomTexture;
	ID3D11ShaderResourceView* randomSRV;
	ID3D11ShaderResourceView* particleTexture;
	ID3D11SamplerState* particleSampler;
	ID3D11BlendState* particleBlendState;
	ID3D11DepthStencilState* particleDepthState;

	// Particle params
	DirectX::XMFLOAT3 particleStartPosition;
	DirectX::XMFLOAT3 particleStartVelocity;
	DirectX::XMFLOAT4 particleStartColor;
	DirectX::XMFLOAT4 particleMidColor;
	DirectX::XMFLOAT4 particleEndColor;
	float particleStartSize;
	float particleMidSize;
	float particleEndSize;

	float particleAgeToSpawn;
	float particleMaxLifetime;
	DirectX::XMFLOAT3 particleConstantAccel;
};