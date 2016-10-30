#include "Renderer.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"


using namespace DirectX;

Renderer::Renderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView)
{
	this->camera = camera;
	this->device = device;
	this->context = context;
	this->backBufferRTV = backBufferRTV;
	this->depthStencilView = depthStencilView;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	AddSampler("default", &samplerDesc);

	SetUpShadows();
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

	typedef std::map<std::string, ID3D11SamplerState*>::iterator sampler_type;
	for (sampler_type iterator = SamplerDictionary.begin(); iterator != SamplerDictionary.end(); iterator++) {
		iterator->second->Release();
	}

	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
}

void Renderer::RenderShadowMap(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights, std::vector<ScenePointLight>* pointLights)
{
	// Set up targets
	context->OMSetRenderTargets(0, 0, shadowDSV);
	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	// Make a viewport to match the render target size
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)shadowMapSize;
	viewport.Height = (float)shadowMapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	// Set up our shadow VS shader
	SimpleVertexShader* shadowShader = GetVertexShader("shadow");
	shadowShader->SetShader();
	shadowShader->SetMatrix4x4("view", directionalLights->at(0).ViewMatrix );
	shadowShader->SetMatrix4x4("projection", shadowDirectionalProjectionMatrix);

	// Turn off pixel shader
	context->PSSetShader(0, 0, 0);

	// Loop through entities and draw them
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (unsigned int i = 0; i < gameEntitys->size(); i++)
	{
		// Grab the data from the first entity's mesh
		GameEntity* ge = gameEntitys->at(i);
		ID3D11Buffer* vb = GetMesh(gameEntitys->at(i)->GetMesh())->GetVertexBuffer();
		ID3D11Buffer* ib = GetMesh(gameEntitys->at(i)->GetMesh())->GetIndexBuffer();

		// Set buffers in the input assembler
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shadowShader->SetMatrix4x4("world", *ge->GetWorld());
		shadowShader->CopyAllBufferData();

		// Finally do the actual drawing
		context->DrawIndexed(GetMesh(gameEntitys->at(i)->GetMesh())->GetIndexCount(), 0, 0);
	}

	// Change everything back
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);
}

void Renderer::DrawOneMaterial(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights, std::vector<ScenePointLight>* pointLights, FLOAT deltaTime, FLOAT totalTime)
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
		blendMode = true;
	}
	else if(blendMode == true)
	{
		context->OMSetBlendState(0, 0, 0);
		blendMode = false;
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
	pixelShader->SetShaderResourceView("Sky", skyBox->GetSRV());

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

void Renderer::DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights, std::vector<ScenePointLight>* pointLights, FLOAT deltaTime, FLOAT totalTime)
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
		blendMode = true;
	}
	else if (blendMode == true)
	{
		float factors[4] = { 1,1,1,1 };
		context->OMSetBlendState(0, factors, 0xFFFFFFFF);
		blendMode = false;
	}

	// Send texture Info
	pixelShader->SetSamplerState("basicSampler", material->GetSamplerState());
	pixelShader->SetShaderResourceView("diffuseTexture", material->GetSRV());
	//pixelShader->SetShaderResourceView("NormalMap", material->GetNormMap());

	// Send Lighting Info
	SceneDirectionalLight firstDirectionalLight = directionalLights->at(0);
	DirectionalLight dLight;
	dLight.AmbientColor = firstDirectionalLight.AmbientColor;
	dLight.DiffuseColor = firstDirectionalLight.DiffuseColor;
	dLight.Direction = firstDirectionalLight.Direction;
	pixelShader->SetData(
		"light",
		&dLight,
		sizeof(DirectionalLight));

	ScenePointLight firstPointLight = pointLights->at(0);
	PointLight pLight;
	pLight.Color = firstPointLight.Color;
	pLight.Position = firstPointLight.Position;

	pixelShader->SetData(
		"pointLight",
		&pLight,
		sizeof(PointLight));

	pixelShader->CopyAllBufferData();

	// Send Geometry
	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
	pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());
	pixelShader->SetShaderResourceView("Sky", skyBox->GetSRV());

	//Do shadow stuff!
	vertexShader->SetMatrix4x4("shadowView", firstDirectionalLight.ViewMatrix);
	vertexShader->SetMatrix4x4("shadowProjection", shadowDirectionalProjectionMatrix);
	pixelShader->SetShaderResourceView("ShadowMap", shadowSRV);
	pixelShader->SetSamplerState("ShadowSampler", GetSampler("shadow"));

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

void Renderer::DrawSkyBox()
{
	CubeMap* sky = skyBox;
	SimpleVertexShader* vertexShader = GetVertexShader("skybox");
	SimplePixelShader* pixelShader = GetPixelShader("skybox");

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertTemp = GetMesh("cube")->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
	context->IASetIndexBuffer(GetMesh("cube")->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Sky", sky->GetSRV());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
	context->RSSetState(sky->rastState);
	context->OMSetDepthStencilState(sky->skyDepthState, 0);

	context->DrawIndexed(GetMesh("cube")->GetIndexCount(), 0, 0);

	// Reset the states!
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
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

void Renderer::AddMaterial(std::string name, std::wstring path, std::string sampler)
{
	ID3D11ShaderResourceView* SRV;
	CreateWICTextureFromFile(device, context, path.c_str(), 0, &SRV);
	Material* mat = new Material(SRV, GetSampler(sampler));
	MaterialDictionary.insert(std::pair<std::string, Material*>(name, mat));
	mat->AddReference();
}

void Renderer::AddMaterial(std::string name, std::wstring path)
{
	AddMaterial(name, path, "default");
}

void Renderer::AddCubeMaterial(std::string name, CubeMap * material)
{
	MaterialDictionary.insert(std::pair<std::string, Material*>(name, material));
	material->AddReference();
}

void Renderer::AddCubeMaterial(std::string name, std::wstring path, std::string sampler, D3D11_RASTERIZER_DESC* rasterDesc, D3D11_DEPTH_STENCIL_DESC* depthStencilDesc)
{
	ID3D11ShaderResourceView* SRV;
	HRESULT error = CreateDDSTextureFromFile(device, path.c_str(), 0, &SRV);
	ID3D11RasterizerState* rastState;
	ID3D11DepthStencilState* depthState;
	device->CreateRasterizerState(rasterDesc, &rastState);
	device->CreateDepthStencilState(depthStencilDesc, &depthState);
	CubeMap* mat = new CubeMap(rastState, depthState, SRV, GetSampler(sampler));
	MaterialDictionary.insert(std::pair<std::string, Material*>(name, mat));
	mat->AddReference();
}

void Renderer::AddCubeMaterial(std::string name, std::wstring path)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Make sure we can see the sky (at max depth)

	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.DepthClipEnable = true;
	AddCubeMaterial(name, path, "default", &rsDesc, &dsDesc);
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

void Renderer::AddSampler(std::string name, D3D11_SAMPLER_DESC * sampleDesc)
{
	ID3D11SamplerState* samplerState;
	device->CreateSamplerState(sampleDesc, &samplerState);
	SamplerDictionary.insert(std::pair<std::string, ID3D11SamplerState*>(name, samplerState));
}

ID3D11SamplerState * Renderer::GetSampler(std::string name)
{
	return SamplerDictionary.at(name);
}

void Renderer::SetSkyBox(std::string name)
{
	skyBox = (CubeMap*)GetMaterial(name);
}

void Renderer::SetUpShadows()
{
	// Get setup for shadows
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	// Release the texture reference since we don't need it
	shadowTexture->Release();

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	AddSampler("shadow", &shadowSampDesc);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	// Orthographic to match the directional light
	//TODO: This shouldn't call direct X math stuff directly. Fix that
	XMMATRIX shProj = XMMatrixOrthographicLH(10, 10, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowDirectionalProjectionMatrix, XMMatrixTranspose(shProj));

	XMMATRIX shProjPersp = XMMatrixPerspectiveLH(10, 10, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowPointProjectionMatrix, XMMatrixTranspose(shProj));
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

