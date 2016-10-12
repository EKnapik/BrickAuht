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
	pixelShader->SetSamplerState("Sampler", material->GetSamplerState());
	pixelShader->SetShaderResourceView("Texture", material->GetSRV());
	//pixelShader->SetShaderResourceView("NormalMap", material->GetNormMap());

	// Send Lighting Info
	struct DirectionalLight directionalLight = DirectionalLight();
	directionalLight.AmbientColor = XMFLOAT4(.1, .1, .1, 1.0);
	directionalLight.DiffuseColor = XMFLOAT4(.8, .8, .1, 1.0);
	directionalLight.Direction = XMFLOAT3(1.0, -1.0, 0.0);
	pixelShader->SetData("dirLight1", &directionalLight, sizeof(DirectionalLight));

	struct DirectionalLight directionalLight2 = DirectionalLight();
	directionalLight2.AmbientColor = XMFLOAT4(.1, .1, .1, 1.0);
	directionalLight2.DiffuseColor = XMFLOAT4(0, 1.0, .1, 1.0);
	directionalLight2.Direction = XMFLOAT3(0.0, 1.0, 0.0);
	pixelShader->SetData("dirLight2", &directionalLight2, sizeof(DirectionalLight));

	struct PointLight pointLight = PointLight();
	pointLight.Color = XMFLOAT4(1.0, 0.0, 0.5, 1.0);
	pointLight.Position = XMFLOAT3(3.0, 3.0, 3.0);
	pixelShader->SetData("pointLight", &pointLight, sizeof(PointLight));

	pixelShader->CopyAllBufferData();

	// Send Geometry
	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());

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

