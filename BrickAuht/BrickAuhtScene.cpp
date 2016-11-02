#include "BrickAuhtScene.h"
#include "Ball.h"

BrickAuhtScene::BrickAuhtScene()
{
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			GameEntity* temp = new GameEntity("cube", "default");

			Ball* ball = new Ball();
			ball->SetEntity(temp);
			ball->kinematics->velocity = VEC3(0, 0, 0);
			ball->kinematics->acceleration = VEC3(0, 0, 0);
			ball->kinematics->SetPosition(VEC3(x - 2.5f, y - 2.5f, 20));
			blocks.push_back(ball);
			GameObjects.push_back(ball);
		}
	}

	GameEntity* BallEntity = new GameEntity("sphere", "white");
	this->ball = new Ball();
	this->ball->SetEntity(BallEntity);
	this->ball->kinematics->velocity = VEC3(0, 0, 5);
	this->ball->kinematics->acceleration = VEC3(0, -2.0f, 0);
	this->ball->kinematics->SetPosition(VEC3(0, 5.0f, 0));
	GameObjects.push_back(this->ball);

	GameEntity* cubeBox = new GameEntity("court", "white");
	cubeBox->SetScale(VEC3(2, 2, 2));
	cubeBox->SetRotation(VEC3(0, PI, 0));
	court = new GameObject();
	court->SetEntity(cubeBox);
	cubeBox->SetPosition(VEC3(0, -1.0f, 0));
	GameObjects.push_back(court);

	GameEntity* temp = new GameEntity("cube", "greenopaque");
	paddle = new Ball();
	paddle->SetEntity(temp);
	paddle->kinematics->SetPosition(VEC3(0, 0, -4.5f));
	GameObjects.push_back(paddle);

	for (int i = 0; i < 6; i++)
	{
		PointLights.push_back(ScenePointLight(
			VEC4(1.0f, 1.0f, 1.0f, 1.0f),
			VEC3(-5, 5, i * 3), 3.0));

		PointLights.push_back(ScenePointLight(
			VEC4(1.0f, 1.0f, 1.0f, 1.0f),
			VEC3(5, 5, i * 3), 3.0));
	}
	playerLight = new ScenePointLight(
		VEC4(1.0f, 0, 0, 1.0f),
		VEC3(0, 0, 0), 3.0);
	PointLights.push_back(*playerLight);

	ballLight = new ScenePointLight(
		VEC4(0, 1.0f, 0, 1.0f),
		VEC3(0, 0, 0), 3.0);
	PointLights.push_back(*ballLight);

	//Big light to light the main court
	/*PointLights.push_back(ScenePointLight(
		VEC4(0.3f, 0.5f, 0.3f, 1.0f),
		VEC3(0, 10, 20), 20.0f));*/

	DirectionalLights.push_back(SceneDirectionalLight(
		VEC4(0.1f, 0.1f, 0.1f, 1.0f),
		VEC4(1.0f, 1.0f, 1.0f, 1.0f),
		VEC3(0, 5, -10)));
}

void BrickAuhtScene::Initialize()
{
}

void BrickAuhtScene::Update()
{
	float paddleSpeed = 5;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		paddle->kinematics->velocity = VEC3(-paddleSpeed, paddle->kinematics->velocity.y, 0);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		paddle->kinematics->velocity = VEC3(paddleSpeed, paddle->kinematics->velocity.y, 0);
	}
	else
	{
		paddle->kinematics->velocity = VEC3(0, paddle->kinematics->velocity.y, 0);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		paddle->kinematics->velocity = VEC3(paddle->kinematics->velocity.x, paddleSpeed, 0);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		paddle->kinematics->velocity = VEC3(paddle->kinematics->velocity.x, -paddleSpeed, 0);
	}
	else
	{
		paddle->kinematics->velocity = VEC3(paddle->kinematics->velocity.x, 0, 0);
	}

	playerLight->Position = paddle->kinematics->GetPosition();
	ballLight->Position = ball->kinematics->GetPosition();

	// Let's add some logic to make the ball bounce off the walls.
	float length = 15.0f;
	float width = 5.0f;
	if (ball->kinematics->GetPosition().x > width)
	{
		ball->kinematics->velocity.x = abs(ball->kinematics->velocity.x) * -1;
	}
	if (ball->kinematics->GetPosition().x < -width)
	{
		ball->kinematics->velocity.x = abs(ball->kinematics->velocity.x);
	}

	/*if (ball->kinematics->GetPosition().y > boxSize)
	{
		ball->kinematics->velocity.y = abs(ball->kinematics->velocity.y) * -1;
	}*/
	if (ball->kinematics->GetPosition().y < 0.0f)
	{
		ball->kinematics->velocity.y = abs(ball->kinematics->velocity.y);
	}
	float wall = 5.0f;
	if (ball->kinematics->GetPosition().z > length + wall)
	{
		ball->kinematics->velocity.z = abs(ball->kinematics->velocity.z) * -1;
	}
	if (ball->kinematics->GetPosition().z < wall)
	{
		ball->kinematics->velocity.z = abs(ball->kinematics->velocity.z);
	}

	VEC3 distanceVec;
	GMath::AddVec3(&distanceVec, &ball->kinematics->GetPosition(),
		(GMath::VectorScale(&GMath::GetVector(&paddle->kinematics->GetPosition()), -1.0f)));

	FLOAT distance;
	GMath::GetMagnitude(&distance, &distanceVec);

	if (distance < ball->radius + paddle->radius)
	{
		GMath::Vec3Normalize(&GMath::GetVector(&distanceVec));
		float velocityMag;
		GMath::GetMagnitude(&velocityMag, &ball->kinematics->velocity);
		GMath::SetVector3(&ball->kinematics->velocity, distanceVec.x, distanceVec.y, distanceVec.z);
		GMath::VectorScale(&ball->kinematics->velocity, velocityMag);

		GMath::GetMagnitude(&velocityMag, &paddle->kinematics->velocity);
		GMath::SetVector3(&paddle->kinematics->velocity, distanceVec.x, distanceVec.y, distanceVec.z);
		GMath::VectorScale(&paddle->kinematics->velocity, -velocityMag);
	}


	for (int p = 0; p < blocks.size(); p++)
	{
		VEC3 distanceVec;
		GMath::AddVec3(&distanceVec, &ball->kinematics->GetPosition(),
			(GMath::VectorScale(&GMath::GetVector(&blocks.at(p)->kinematics->GetPosition()), -1.0f)));

		FLOAT distance;
		GMath::GetMagnitude(&distance, &distanceVec);

		if (distance < ball->radius + blocks.at(p)->radius)
		{
			GMath::Vec3Normalize(&GMath::GetVector(&distanceVec));
			float velocityMag;
			GMath::GetMagnitude(&velocityMag, &ball->kinematics->velocity);
			GMath::SetVector3(&ball->kinematics->velocity, distanceVec.x, distanceVec.y, distanceVec.z);
			GMath::VectorScale(&ball->kinematics->velocity, velocityMag);

			blocks.at(p)->ToDelete = true;
			blocks.erase(blocks.begin() + p);
			p--;
		}
	}
}

BrickAuhtScene::~BrickAuhtScene()
{
	delete playerLight;
	delete ballLight;
}

