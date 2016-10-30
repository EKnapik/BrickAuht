#pragma once
#include <map>
#include <DirectXMath.h>
#include "SimpleShader.h"
#include "DXMathImpl.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"



class Renderer
{
public:
	Renderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView);
	~Renderer();

	void RenderShadowMap(std::vector<GameEntity*>* gameEntitys);

	virtual void Render(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime) {};

	virtual void DrawOneMaterial(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime);

	void DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime);

	void DrawSkyBox();

	void AddMesh(std::string name, Mesh* mesh);
	void AddMesh(std::string name, std::string path);

	Mesh* GetMesh(std::string name);
	Material* GetMaterial(std::string name);

	void AddMaterial(std::string name, Material* material);
	void AddMaterial(std::string name, std::wstring path, std::string sampler);
	///<summary>
	/// Adds a material with the specified name and path. Uses a Default sampler description
	///</summary>
	void AddMaterial(std::string name, std::wstring path);

	void AddCubeMaterial(std::string name, CubeMap* material);
	void AddCubeMaterial(std::string name, std::wstring path, std::string sampler, D3D11_RASTERIZER_DESC* rasterDesc, D3D11_DEPTH_STENCIL_DESC* depthStencilDesc);
	///<summary>
	/// Adds a material with the specified name and path. Uses a Default sampler description
	///</summary>
	void AddCubeMaterial(std::string name, std::wstring path);

	void AddVertexShader(std::string name, std::wstring path);
	void AddPixelShader(std::string name, std::wstring path);

	void AddSampler(std::string name, D3D11_SAMPLER_DESC* sampleDesc);
	ID3D11SamplerState* GetSampler(std::string name);

	void SetSkyBox(std::string name);

	int width, height;

protected:

	void SetUpShadows();

	Camera *camera;

	ID3D11Device*			device;
	ID3D11DeviceContext*	context;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11RasterizerState* shadowRasterizer;

	std::map<std::string, Mesh*>				MeshDictionary;
	std::map<std::string, Material*>			MaterialDictionary;
	std::map<std::string, SimpleVertexShader*>	VertexShaderDictionary;
	std::map<std::string, SimplePixelShader*>	PixelShaderDictionary;
	std::map<std::string, ID3D11SamplerState*> SamplerDictionary;
	
	SimpleVertexShader* GetVertexShader(std::string name);
	SimplePixelShader* GetPixelShader(std::string name);

	CubeMap* skyBox = nullptr;

	bool blendMode = false;

	int shadowMapSize = 1080;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
};

