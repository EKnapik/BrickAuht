#include "BrickAuhtScene.h"
#include "Ball.h"

BrickAuhtScene::BrickAuhtScene()
{
	{
		GameEntity* temp = new GameEntity("sphere", "electricity");

		Ball* ball = new Ball();
		ball->SetEntity(temp);
		ball->kinematics->velocity = VEC3(0, 1, 5);
		ball->kinematics->acceleration = VEC3(0, 0, 0);
		ball->kinematics->SetPosition(VEC3(0, 0, 0));
		balls.push_back(ball);
		GameObjects.push_back(ball);
	}
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			GameEntity* temp = new GameEntity("cube", "greenopaque");

			Ball* ball = new Ball();
			ball->SetEntity(temp);
			ball->kinematics->velocity = VEC3(0, 0, 0);
			ball->kinematics->acceleration = VEC3(0, 0, 0);
			ball->kinematics->SetPosition(VEC3(x - 2.5f, y - 2.5f, 5));
			blocks.push_back(ball);
			GameObjects.push_back(ball);
		}
	}

	GameEntity* cubeBox = new GameEntity("cube", "default");
	cubeBox->SetScale(VEC3(10, 10, 10));
	GameObject* cubeObject = new GameObject();
	cubeObject->SetEntity(cubeBox);
	cubeBox->SetPosition(VEC3(0, -8.0f, 0));
	GameObjects.push_back(cubeObject);

	GameEntity* temp = new GameEntity("cube", "greenopaque");
	paddle = new Ball();
	paddle->SetEntity(temp);
	paddle->kinematics->SetPosition(VEC3(0, 0, -4.5f));
	balls.push_back(paddle);
	GameObjects.push_back(paddle);

	PointLights.push_back(ScenePointLight(
		VEC4(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f, 1.0f),
		VEC3(0, 0, -5)));

	DirectionalLights.push_back(SceneDirectionalLight(
		VEC4(0.1f, 0.1f, 0.1f, 1.0f),
		VEC4(71.0f / 255.0f, 28.0f / 255.0f, 1.0f / 255.0f, 1.0f),
		VEC3(0, 20, -20)));
}

void BrickAuhtScene::Initialize()
{
}

void BrickAuhtScene::Update()
{
	float paddleSpeed = 3;
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

	// Let's add some logic to make the balls bounce off the walls.
	for (int i = 0; i < balls.size(); i++)
	{
		float boxSize = 2.5f;
		if (balls.at(i)->kinematics->GetPosition().x > boxSize)
		{
			balls.at(i)->kinematics->velocity.x = abs(balls.at(i)->kinematics->velocity.x) * -1;
		}
		if (balls.at(i)->kinematics->GetPosition().x < -boxSize)
		{
			balls.at(i)->kinematics->velocity.x = abs(balls.at(i)->kinematics->velocity.x);
		}

		if (balls.at(i)->kinematics->GetPosition().y > boxSize)
		{
			balls.at(i)->kinematics->velocity.y = abs(balls.at(i)->kinematics->velocity.y) * -1;
		}
		if (balls.at(i)->kinematics->GetPosition().y < -boxSize)
		{
			balls.at(i)->kinematics->velocity.y = abs(balls.at(i)->kinematics->velocity.y);
		}

		if (balls.at(i)->kinematics->GetPosition().z > boxSize * 2)
		{
			balls.at(i)->kinematics->velocity.z = abs(balls.at(i)->kinematics->velocity.z) * -1;
		}

		for (int p = i + 1; p < balls.size(); p++)
		{
			VEC3 distanceVec;
			GMath::AddVec3(&distanceVec, &balls.at(i)->kinematics->GetPosition(),
				(GMath::VectorScale(&GMath::GetVector(&balls.at(p)->kinematics->GetPosition()), -1.0f)));

			FLOAT distance;
			GMath::GetMagnitude(&distance, &distanceVec);

			if (distance < balls.at(i)->radius + balls.at(p)->radius)
			{
				GMath::Vec3Normalize(&GMath::GetVector(&distanceVec));
				float velocityMag;
				GMath::GetMagnitude(&velocityMag, &balls.at(i)->kinematics->velocity);
				GMath::SetVector3(&balls.at(i)->kinematics->velocity, distanceVec.x, distanceVec.y, distanceVec.z);
				GMath::VectorScale(&balls.at(i)->kinematics->velocity, velocityMag);

				GMath::GetMagnitude(&velocityMag, &balls.at(p)->kinematics->velocity);
				GMath::SetVector3(&balls.at(p)->kinematics->velocity, distanceVec.x, distanceVec.y, distanceVec.z);
				GMath::VectorScale(&balls.at(p)->kinematics->velocity, -velocityMag);
			}

		}
		for (int p = i + 1; p < blocks.size(); p++)
		{
			VEC3 distanceVec;
			GMath::AddVec3(&distanceVec, &balls.at(i)->kinematics->GetPosition(),
				(GMath::VectorScale(&GMath::GetVector(&blocks.at(p)->kinematics->GetPosition()), -1.0f)));

			FLOAT distance;
			GMath::GetMagnitude(&distance, &distanceVec);

			if (distance < balls.at(i)->radius + blocks.at(p)->radius)
			{
				GMath::Vec3Normalize(&GMath::GetVector(&distanceVec));
				float velocityMag;
				GMath::GetMagnitude(&velocityMag, &balls.at(i)->kinematics->velocity);
				GMath::SetVector3(&balls.at(i)->kinematics->velocity, distanceVec.x, distanceVec.y, distanceVec.z);
				GMath::VectorScale(&balls.at(i)->kinematics->velocity, velocityMag);

				blocks.at(p)->ToDelete = true;
				blocks.erase(blocks.begin() + p);
				p--;
			}

		}
	}
}
