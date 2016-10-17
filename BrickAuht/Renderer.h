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
	Renderer(Camera *camera, ID3D11DeviceContext *context,
		ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView,
		ID3D11Device* device);
	~Renderer();

	void DrawOneMaterial(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime);

	void DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime);

	void AddMesh(std::string name, Mesh* mesh);
	void AddMesh(std::string name, std::string path);

	void AddMaterial(std::string name, Material* material);
	void AddMaterial(std::string name, std::wstring path, D3D11_SAMPLER_DESC* sampleDesc);
	///<summary>
	/// Adds a material with the specified name and path. Uses a Default sampler description
	///</summary>
	void AddMaterial(std::string name, std::wstring path);

	void AddVertexShader(std::string name, std::wstring path);
	void AddPixelShader(std::string name, std::wstring path);

private:
	Camera *camera;

	ID3D11Device*			device;
	ID3D11DeviceContext*	context;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;

	std::map<std::string, Mesh*>				MeshDictionary;
	std::map<std::string, Material*>			MaterialDictionary;
	std::map<std::string, SimpleVertexShader*>	VertexShaderDictionary;
	std::map<std::string, SimplePixelShader*>	PixelShaderDictionary;

	Mesh* GetMesh(std::string name);
	Material* GetMaterial(std::string name);
	SimpleVertexShader* GetVertexShader(std::string name);
	SimplePixelShader* GetPixelShader(std::string name);
};

