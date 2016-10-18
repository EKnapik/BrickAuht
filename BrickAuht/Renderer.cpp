#include "Renderer.h"
#include "WICTextureLoader.h"


using namespace DirectX;

Renderer::Renderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView)
{
	this->camera = camera;
	this->device = device;
	this->context = context;
	this->backBufferRTV = backBufferRTV;
	this->depthStencilView = depthStencilView;
}


Renderer::~Renderer()
{
	typedef std::map<std::string, Mesh*>::iterator mesh_type;
	for (mesh_type iterator = MeshDictionary.begin(); iterator != MeshDictionary.end(); iterator++) {
		iterator->second->Release();
	}

	typedef std::map<std::string, Material*>::iterator material_type;
	for (material_type iterator = MaterialDictionary.begin(); iterator != MaterialDictionary.end(); iterator++) {
		iterator->second->Release();
	}

	typedef std::map<std::string, SimplePixelShader*>::iterator pixel_type;
	for (pixel_type iterator = PixelShaderDictionary.begin(); iterator != PixelShaderDictionary.end(); iterator++) {
		delete iterator->second;
	}

	typedef std::map<std::string, SimpleVertexShader*>::iterator vertex_type;
	for (vertex_type iterator = VertexShaderDictionary.begin(); iterator != VertexShaderDictionary.end(); iterator++) {
		delete iterator->second;
	}
}

void Renderer::DrawOneMaterial(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime)
{
	// Background color (Cornflower Blue)
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer
	// TODO THIS SHOULD NOT BE HERE!!!!!!!!!!!!!
	// THIS CAN RUIN THE ENTIRE SCENE IF CALLED MULTIPLE TIMES
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	if (gameEntitys->size() == 0) return;
	Material* material = GetMaterial(gameEntitys->at(0)->GetMaterial());
	SimpleVertexShader* vertexShader = GetVertexShader(gameEntitys->at(0)->vertexShader);
	SimplePixelShader* pixelShader = GetPixelShader(gameEntitys->at(0)->pixelShader);
	vertexShader->SetShader();
	pixelShader->SetShader();

	if (material->transparency == true)
	{
		D3D11_BLEND_DESC bd = {};
		bd.AlphaToCoverageEnable = false;
		bd.IndependentBlendEnable = false;
		bd.RenderTarget[0].BlendEnable = true;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ID3D11BlendState* blendState;
		device->CreateBlendState(&bd, &blendState);

		float factors[4] = { 1,1,1,1 };
		context->OMSetBlendState(
			blendState,
			factors,
			0xFFFFFFFF);
	}

	// Send texture Info
	pixelShader->SetSamplerState("basicSampler", material->GetSamplerState());
	pixelShader->SetShaderResourceView("diffuseTexture", material->GetSRV());
	//pixelShader->SetShaderResourceView("NormalMap", material->GetNormMap());

	// Send Lighting Info
	DirectionalLight dLight;
	dLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 0.0f);
	dLight.DiffuseColor = VEC4(0, 0, 1, 0);
	dLight.Direction = VEC3(1, -1, 0);
	pixelShader->SetData(
		"light",
		&dLight,
		sizeof(DirectionalLight));

	// Create a ground light so every object is lit a little bit from the ground
	DirectionalLight gLight;
	gLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
	gLight.DiffuseColor = VEC4(71.0f / 255.0f, 28.0f / 255.0f, 1.0f / 255.0f, 1.0f);
	gLight.Direction = VEC3(0, 1, 0);

	pixelShader->SetData(
		"groundLight",
		&gLight,
		sizeof(DirectionalLight));

	PointLight pLight;
	pLight.Color = VEC4(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f, 1.0f);
	pLight.Position = VEC3(0, 0, -5);

	pixelShader->SetData(
		"pointLight",
		&pLight,
		sizeof(PointLight));

	pixelShader->CopyAllBufferData();

	// Send Geometry
	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
	pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Mesh* meshTmp;
	for (int i = 0; i < gameEntitys->size(); i++) {
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();

		meshTmp = GetMesh(gameEntitys->at(i)->GetMesh());
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

}

void Renderer::DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime)
{
	// Background color (Cornflower Blue)
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	if (gameEntitys->size() == 0) return;

	for (int i = 0; i < gameEntitys->size(); i++) {
	Material* material = GetMaterial(gameEntitys->at(i)->GetMaterial());
	SimpleVertexShader* vertexShader = GetVertexShader(gameEntitys->at(i)->vertexShader);
	SimplePixelShader* pixelShader = GetPixelShader(gameEntitys->at(i)->pixelShader);
	vertexShader->SetShader();
	pixelShader->SetShader();

	// Send texture Info
	pixelShader->SetSamplerState("basicSampler", material->GetSamplerState());
	pixelShader->SetShaderResourceView("diffuseTexture", material->GetSRV());
	//pixelShader->SetShaderResourceView("NormalMap", material->GetNormMap());

	// Send Lighting Info
	DirectionalLight dLight;
	dLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 0.0f);
	dLight.DiffuseColor = VEC4(0, 0, 1, 0);
	dLight.Direction = VEC3(1, -1, 0);
	pixelShader->SetData(
		"light",
		&dLight,
		sizeof(DirectionalLight));

	// Create a ground light so every object is lit a little bit from the ground
	DirectionalLight gLight;
	gLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
	gLight.DiffuseColor = VEC4(71.0f / 255.0f, 28.0f / 255.0f, 1.0f / 255.0f, 1.0f);
	gLight.Direction = VEC3(0, 1, 0);

	pixelShader->SetData(
		"groundLight",
		&gLight,
		sizeof(DirectionalLight));

	PointLight pLight;
	pLight.Color = VEC4(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f, 1.0f);
	pLight.Position = VEC3(0, 0, -5);

	pixelShader->SetData(
		"pointLight",
		&pLight,
		sizeof(PointLight));

	pixelShader->CopyAllBufferData();

	// Send Geometry
	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
	pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Mesh* meshTmp;
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();

		meshTmp = GetMesh(gameEntitys->at(i)->GetMesh());
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}
}

void Renderer::AddMesh(std::string name, Mesh * mesh)
{
	MeshDictionary.insert(std::pair<std::string, Mesh*>(name, mesh));
	mesh->AddReference();
}

void Renderer::AddMesh(std::string name, std::string path)
{
	Mesh* mesh = new Mesh(path, device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>(name, mesh));
	mesh->AddReference();
}

void Renderer::AddMaterial(std::string name, Material * material)
{
	MaterialDictionary.insert(std::pair<std::string, Material*>(name, material));
	material->AddReference();
}

void Renderer::AddMaterial(std::string name, std::wstring path, D3D11_SAMPLER_DESC* sampleDesc)
{
	ID3D11ShaderResourceView* SRV;
	CreateWICTextureFromFile(device, context, path.c_str(), 0, &SRV);
	ID3D11SamplerState* samplerState;
	device->CreateSamplerState(sampleDesc, &samplerState);
	Material* mat = new Material(SRV, samplerState);
	MaterialDictionary.insert(std::pair<std::string, Material*>(name, mat));
	mat->AddReference();
}

void Renderer::AddMaterial(std::string name, std::wstring path)
{
	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	AddMaterial(name, path, &sampleDesc);
}

void Renderer::AddVertexShader(std::string name, std::wstring path)
{
	std::wstring debug = L"Debug/";
	debug += path;
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	
	if (!vertexShader->LoadShaderFile(debug.c_str()))
	{
		vertexShader->LoadShaderFile(path.c_str());
	}
	VertexShaderDictionary.insert(std::pair<std::string, SimpleVertexShader*>(name, vertexShader));
}

void Renderer::AddPixelShader(std::string name, std::wstring path)
{
	std::wstring debug = L"Debug/";
	debug += path;
	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFile(debug.c_str()))
	{
		pixelShader->LoadShaderFile(path.c_str());
	}
	PixelShaderDictionary.insert(std::pair<std::string, SimplePixelShader*>(name, pixelShader));
}

Mesh * Renderer::GetMesh(std::string name)
{
	return MeshDictionary.at(name);
}

Material * Renderer::GetMaterial(std::string name)
{
	return MaterialDictionary.at(name);
}

SimpleVertexShader * Renderer::GetVertexShader(std::string name)
{
	return VertexShaderDictionary.at(name);
}

SimplePixelShader * Renderer::GetPixelShader(std::string name)
{
	return PixelShaderDictionary.at(name);
}

