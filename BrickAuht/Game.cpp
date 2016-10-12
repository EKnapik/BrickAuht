#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "Ball.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore( 
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class

	for (int i = 0; i < entities.size(); i++)
	{
		entities.at(i)->Release();
	}

	typedef std::map<std::string, Mesh*>::iterator it_type;
	for (it_type iterator = MeshDictionary.begin(); iterator != MeshDictionary.end(); iterator++) {
		iterator->second->Release();
	}

	delete renderer;
	delete camera;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	LoadShaders();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	camera = new Camera(width, height);

	// TODO: Renderer should only take the context and then create the buffers it needs
	renderer = new Renderer(camera, context, backBufferRTV, depthStencilView);

	light.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
	light.DiffuseColor = VEC4(0, 0, 1, 1);
	light.Direction = VEC3(1, -1, 0);

	printf("\nShift + number keys will change the shape!");
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
void Game::LoadShaders()
{
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");		

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	//Lets load two textures now as well
	ID3D11ShaderResourceView* SRV;
	CreateWICTextureFromFile(device, context, L"Assets/Textures/MetalPlate.png", 0, &SRV);

	ID3D11SamplerState* samplerState;
	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampleDesc, &samplerState);

	material = new Material(vertexShader, pixelShader, SRV, samplerState);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	Mesh* cone = new Mesh("Assets/cone.obj", device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>("cone", cone));
	cone->AddReference();

	Mesh* cube = new Mesh("Assets/cube.obj", device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>("cube", cube));
	cube->AddReference();

	Mesh* cylinder = new Mesh("Assets/cylinder.obj", device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>("cylinder", cylinder));
	cylinder->AddReference();

	Mesh* helix = new Mesh("Assets/helix.obj", device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>("helix", helix));
	helix->AddReference();

	Mesh* sphere = new Mesh("Assets/sphere.obj", device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>("sphere", sphere));
	sphere->AddReference();

	Mesh* torus = new Mesh("Assets/torus.obj", device);
	MeshDictionary.insert(std::pair<std::string, Mesh*>("torus", torus));
	torus->AddReference();

	for (int i = 0; i < 10; i++)
	{
		GameEntity* firstEntity = new GameEntity(sphere, material);

		Ball* ball = new Ball();
		ball->SetEntity(firstEntity);
		ball->kinematics->velocity = VEC3(0, 0, 0);
		ball->kinematics->acceleration = VEC3(-2.5f + i * 0.5f, -(i + 1), -2.5f + i * 0.5f);
		ball->kinematics->SetPosition(VEC3(-5 + i, 4, 0));
		gameManager.AddObject(ball);
	}

	//entities.push_back(firstEntity);
	//firstEntity->AddReference();
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(camera->GetProjection(), XMMatrixTranspose(P)); // Transpose for HLSL!
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	gameManager.Update(deltaTime);

	if (entities.size() > 0 && GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		if (GetAsyncKeyState('1') & 0x8000)
		{
			entities.at(0)->SwapMesh(MeshDictionary.at("cone"));
		}
		else if (GetAsyncKeyState('2') & 0x8000)
		{
			entities.at(0)->SwapMesh(MeshDictionary.at("cube"));
		}
		else if (GetAsyncKeyState('3') & 0x8000)
		{
			entities.at(0)->SwapMesh(MeshDictionary.at("cylinder"));
		}
		else if (GetAsyncKeyState('4') & 0x8000)
		{
			entities.at(0)->SwapMesh(MeshDictionary.at("helix"));
		}
		else if (GetAsyncKeyState('5') & 0x8000)
		{
			entities.at(0)->SwapMesh(MeshDictionary.at("sphere"));
		}
		else if (GetAsyncKeyState('6') & 0x8000)
		{
			entities.at(0)->SwapMesh(MeshDictionary.at("torus"));
		}
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	for (int i = 0; i < gameManager.GameObjects.size(); i++)
	{
		// Prepare data will copy all buffer data, so as long as this comes before that we 
		// are safe to set values in the shader.
		gameManager.GameObjects.at(i)->entity->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition",
			*camera->GetPosition());
		gameManager.GameObjects.at(i)->entity->PrepareShader(camera->GetView(), camera->GetProjection(), &light);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* vBuffer = gameManager.GameObjects.at(i)->entity->GetMesh()->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
		context->IASetIndexBuffer(gameManager.GameObjects.at(i)->entity->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		// Finally do the actual drawing
		context->DrawIndexed(
			gameManager.GameObjects.at(i)->entity->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking. 
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	mouseDown = true;
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	mouseDown = false;
	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (mouseDown)
	{
		FLOAT scalar = .005;
		camera->RotateXY((y - prevMousePos.y) * scalar, (x - prevMousePos.x) * scalar);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any code here...
}
#pragma endregion