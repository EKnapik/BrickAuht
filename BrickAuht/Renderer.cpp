#include "Renderer.h"


using namespace DirectX;

Renderer::Renderer(Camera *camera, ID3D11DeviceContext *context,
	ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView)
{
	this->camera = camera;
	this->context = context;
	this->backBufferRTV = backBufferRTV;
	this->depthStencilView = depthStencilView;
}


Renderer::~Renderer()
{
}

void Renderer::DrawOneMaterial(std::vector<GameEntity*>* gameEntitys, INT numEntities, FLOAT deltaTime, FLOAT totalTime)
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

	if (numEntities == 0) return;
	Material* material = gameEntitys->at(0)->GetMaterial();
	SimpleVertexShader* vertexShader = material->GetVertexShader();
	SimplePixelShader* pixelShader = material->GetPixelShader();
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
	material->GetPixelShader()->SetData(
		"light",
		&dLight,
		sizeof(DirectionalLight));

	// Create a ground light so every object is lit a little bit from the ground
	DirectionalLight gLight;
	gLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
	gLight.DiffuseColor = VEC4(71.0f / 255.0f, 28.0f / 255.0f, 1.0f / 255.0f, 1.0f);
	gLight.Direction = VEC3(0, 1, 0);

	material->GetPixelShader()->SetData(
		"groundLight",
		&gLight,
		sizeof(DirectionalLight));

	PointLight pLight;
	pLight.Color = VEC4(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f, 1.0f);
	pLight.Position = VEC3(0, 0, -5);

	HRESULT result = material->GetPixelShader()->SetData(
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
	for (int i = 0; i < numEntities; i++) {
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();

		meshTmp = gameEntitys->at(i)->GetMesh();
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

}

