#include "ParticleEmitter.h"
#include <time.h>

using namespace GMath;

ParticleEmitter::ParticleEmitter(ID3D11Device * device, SimpleVertexShader * particleVS, SimplePixelShader * particlePS, SimpleGeometryShader * particleGS, SimpleVertexShader * spawnVS, SimpleGeometryShader * spawnGS, ID3D11ShaderResourceView * texture, ID3D11SamplerState * sampler, ID3D11BlendState * particleBlendState, ID3D11DepthStencilState * particleDepthState)
{
	particleTexture = texture;
	particleSampler = sampler;

	this->particleVS = particleVS;
	this->particleGS = particleGS;
	this->particlePS = particlePS;

	this->spawnVS = spawnVS;
	this->spawnGS = spawnGS;

	this->particleBlendState = particleBlendState;
	this->particleDepthState = particleDepthState;

	// Particle setup
	particleStartPosition = VEC3(2.0f, 0, 0);
	particleStartVelocity = VEC3(-2.0f, 2.0f, 0);
	particleStartColor = VEC4(1, 0.1f, 0.1f, 0.2f);
	particleMidColor = VEC4(1, 1, 0.1f, 0.1f);
	particleEndColor = VEC4(1, 0.6f, 0.1f, 0);
	particleStartSize = 0.1f;
	particleMidSize = 5.0f;
	particleEndSize = 5.0f;

	particleAgeToSpawn = 0.00001f;
	particleMaxLifetime = 5.0f;
	particleConstantAccel = VEC3(0, -1.0f, 0);

	// Particle geometry
	// Set up the vertices we want to put into the Vertex Buffer
	ParticleVertex vertices[1];
	vertices[0].Type = 0;
	vertices[0].Age = 0.0f;
	vertices[0].StartPosition = particleStartPosition;
	vertices[0].StartVelocity = particleStartVelocity;
	vertices[0].StartColor = particleStartColor;
	vertices[0].MidColor = particleMidColor;
	vertices[0].EndColor = particleEndColor;
	vertices[0].StartMidEndSize = VEC3(
		particleStartSize,
		particleMidSize,
		particleEndSize);

	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(ParticleVertex) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
	device->CreateBuffer(&vbd, &initialVertexData, &particleVB);

	// Set up "random" stuff -------------------------------------
	unsigned int randomTextureWidth = 1024;

	// Random data for the 1D texture
	srand((unsigned int)time(0));
	std::vector<float> data(randomTextureWidth * 4);
	for (unsigned int i = 0; i < randomTextureWidth * 4; i++)
		data[i] = rand() / (float)RAND_MAX * 2.0f - 1.0f;

	// Set up texture
	D3D11_TEXTURE1D_DESC textureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = 100;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = (void*)&data[0];
	initData.SysMemPitch = randomTextureWidth * sizeof(float) * 4;
	initData.SysMemSlicePitch = 0;
	device->CreateTexture1D(&textureDesc, &initData, &randomTexture);

	// Set up SRV for texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	srvDesc.Texture1D.MipLevels = 1;
	srvDesc.Texture1D.MostDetailedMip = 0;
	device->CreateShaderResourceView(randomTexture, &srvDesc, &randomSRV);

	// Create SO buffers
	spawnGS->CreateCompatibleStreamOutBuffer(&soBufferRead, 1000000);
	spawnGS->CreateCompatibleStreamOutBuffer(&soBufferWrite, 1000000);
	spawnFlip = false;
	frameCount = 0;
}

ParticleEmitter::~ParticleEmitter()
{
	particleVB->Release();
	randomSRV->Release();
	randomTexture->Release();
	soBufferRead->Release();
	soBufferWrite->Release();
}

void ParticleEmitter::Draw(ID3D11DeviceContext * context, Camera * camera, float deltaTime, float totalTime)
{
	// Spawn particles
	DrawSpawn(context, deltaTime, totalTime);

	// Draw particles ----------------------------------------------------

	particleGS->SetMatrix4x4("world", MAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)); // Identity
	particleGS->SetMatrix4x4("view", *camera->GetView());
	particleGS->SetMatrix4x4("projection", *camera->GetProjection());
	particleGS->CopyAllBufferData();

	particleVS->SetFloat3("acceleration", particleConstantAccel);
	particleVS->SetFloat("maxLifetime", particleMaxLifetime);
	particleVS->CopyAllBufferData();

	particlePS->SetSamplerState("trilinear", particleSampler);
	particlePS->SetShaderResourceView("particleTexture", particleTexture);
	particlePS->CopyAllBufferData();

	particleVS->SetShader();
	particlePS->SetShader();
	particleGS->SetShader();

	// Set up states
	float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(particleBlendState, factor, 0xffffffff);
	context->OMSetDepthStencilState(particleDepthState, 0);

	// Set buffers
	UINT particleStride = sizeof(ParticleVertex);
	UINT particleOffset = 0;
	context->IASetVertexBuffers(0, 1, &soBufferRead, &particleStride, &particleOffset);

	// Draw auto - draws based on current stream out buffer
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->DrawAuto();

	// Unset Geometry Shader for next frame and reset states
	context->GSSetShader(0, 0, 0);
	context->OMSetBlendState(0, factor, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);

	// Reset topology
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleEmitter::SwapSOBuffers()
{
	ID3D11Buffer* temp = soBufferRead;
	soBufferRead = soBufferWrite;
	soBufferWrite = temp;
}

void ParticleEmitter::DrawSpawn(ID3D11DeviceContext * context, float deltaTime, float totalTime)
{
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;

	// Set/unset correct shaders
	// Set the delta time for the spawning
	spawnGS->SetFloat("dt", deltaTime);
	spawnGS->SetFloat("ageToSpawn", particleAgeToSpawn);
	spawnGS->SetFloat("maxLifetime", particleMaxLifetime);
	spawnGS->SetFloat("totalTime", totalTime);
	spawnGS->SetSamplerState("randomSampler", particleSampler);
	spawnGS->SetShaderResourceView("randomTexture", randomSRV);
	spawnGS->SetShader();
	spawnGS->CopyAllBufferData();

	spawnVS->SetShader();
	spawnVS->CopyAllBufferData();

	context->PSSetShader(0, 0, 0); // No pixel shader needed

								   // Unbind vertex buffers (incase)
	ID3D11Buffer* unset = 0;
	context->IASetVertexBuffers(0, 1, &unset, &stride, &offset);

	// First frame?
	if (frameCount == 0)
	{
		// Draw using the seed vertex
		context->IASetVertexBuffers(0, 1, &particleVB, &stride, &offset);
		context->SOSetTargets(1, &soBufferWrite, &offset);
		context->Draw(1, 0);
		frameCount++;
	}
	else
	{
		// Draw using the buffers
		context->IASetVertexBuffers(0, 1, &soBufferRead, &stride, &offset);
		context->SOSetTargets(1, &soBufferWrite, &offset);
		context->DrawAuto();
	}

	// Unbind SO targets and shader
	SimpleGeometryShader::UnbindStreamOutStage(context);
	context->GSSetShader(0, 0, 0);

	// Swap after draw
	SwapSOBuffers();
}
