#include "BrickAuhtScene.h"
#include "Ball.h"
#include "Game.h"

BrickAuhtScene::BrickAuhtScene()
{
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			for (int z = 0; z < 8; z++)
			{
				GameEntity* temp;
				int random = rand() % 5;
				switch (random)
				{
				case 0:
					temp = new GameEntity("panel", "electricity");
					break;
				case 1:
					temp = new GameEntity("panel", "greenopaque");
					break;
				case 2:
					temp = new GameEntity("panel", "gridclip");
					break;
				case 3:
					temp = new GameEntity("panel", "white");
					break;
				case 4:
					temp = new GameEntity("panel", "ascii");
					break;
				default:
					temp = new GameEntity("panel", "default");
					break;
				}

				temp->SetScale(VEC3(0.15f, 0.15f, 0.15f));
				temp->SetRotation(VEC3(PI / 2, 0, 0));
				Panel* panel = new Panel();
				panel->SetEntity(temp);
				panel->entity->SetPosition(VEC3(-5 + x * 2 + 3.0f / (float)(rand() % (int)(z + 1)), 
					y * 2,
					22 - z * 1.25f - 1 / (float)(rand() % 3)));
				panel->SetWidthHeight(0.5f, 0.5f);
				panel->AddComponent(new PostProcessComponent());
				blocks.push_back(panel);
				GameObjects.push_back(panel);
			}
		}
	}

	GameEntity* cubeBox = new GameEntity("bbcourt", "wood");
	cubeBox->SetRotation(VEC3(0, 0, 0));
	court = new GameObject();
	court->SetEntity(cubeBox);
	cubeBox->SetScale(VEC3(0.25f, 0.25f, 0.25f));
	cubeBox->SetPosition(VEC3(4.5f, -1.0f, 12.0f));
	GameObjects.push_back(court);

	for (int i = 0; i < 9; i++)
	{
		for (int z = 0; z < 2; z++)
		{
			ScenePointLight* first = new ScenePointLight(
				VEC4(1.0f, 0.2f, 0.2f, 1.0f),
				VEC3(-4 + z * 3.0f, 1, i * 4 - 4), 7.5);

			ScenePointLight* second = new ScenePointLight(
				VEC4(0.2f, 0.2f, 1.0f, 1.0f),
				VEC3(4 - z * 3.0f, 1, i * 4 - 4), 7.5);

			PointLights.push_back(first);
			PointLights.push_back(second);
			crazyLights.push_back(first);
			crazyLights.push_back(second);
		}
	}

	playerLight = new ScenePointLight(
		VEC4(1.0f, 0.5f, 0, 1.0f),
		VEC3(0, 0, 0), 6.0);
	PointLights.push_back(playerLight);

	//ballLight = new ScenePointLight(
	//	VEC4(0.1f, 1.0f, 0.1f, 1.0f),
	//	VEC3(0, 0, 0), 6.0);
	//PointLights.push_back(ballLight);

	//Big light to light the main court
	PointLights.push_back(new ScenePointLight(
		VEC4(0.3f, 0.5f, 0.3f, 1.0f),
		VEC3(0, 10, 20), 20.0f));

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
	playerLight->Position = *Game::GetCamera()->GetPosition();

	if (blocks.size() == 0)
	{
		Game::levelstate = LEVEL_STATE::WIN;
	}

	Shoot();
	int width = 7.5;
	int length = 30;
	for (int b = 0; b < balls.size(); b++)
	{
		if (balls.at(b)->kinematics->GetPosition().x < -width)
		{
			balls.at(b)->kinematics->velocity.x = abs(balls.at(b)->kinematics->velocity.x);
		}
		if (balls.at(b)->kinematics->GetPosition().x > width)
		{
			balls.at(b)->kinematics->velocity.x = -balls.at(b)->kinematics->velocity.x;
		}
		if (balls.at(b)->kinematics->GetPosition().z > length)
		{
			MarkForDelete(balls.at(b));
			balls.erase(balls.begin() + b);
			b--;
			continue;
		}
		if (balls.at(b)->kinematics->GetPosition().y < 0.0f)
		{
			balls.at(b)->kinematics->velocity.y = abs(balls.at(b)->kinematics->velocity.y);
		}

		for (int p = 0; p < blocks.size(); p++)
		{
			VEC3 distanceVec;
			GMath::AddVec3(&distanceVec, &balls.at(b)->kinematics->GetPosition(),
				(GMath::VectorScale(&GMath::GetVector(&blocks.at(p)->entity->GetPosition()), -1.0f)));

			FLOAT distance;
			GMath::GetMagnitude(&distance, &distanceVec);

			if (distance < balls.at(b)->radius + blocks.at(p)->width)
			{
				GMath::Vec3Normalize(&GMath::GetVector(&distanceVec));
				float velocityMag;
				GMath::GetMagnitude(&velocityMag, &balls.at(b)->kinematics->velocity);
				VEC3 bounce;
				GMath::SetVector3(&bounce, distanceVec.x, distanceVec.y, distanceVec.z);
				GMath::VectorScale(&bounce, 3.0f);
				GMath::AddVec3(&balls.at(b)->kinematics->velocity, &balls.at(b)->kinematics->velocity, &bounce);
				MarkForDelete(blocks.at(p));
				blocks.erase(blocks.begin() + p);
				p--;
			}
		}
	}

	for (int i = 0; i < crazyLights.size(); i++)
	{
		float random = (rand() % 2) ? 0.1f : -0.1f;
		random = crazyLights.at(i)->Radius.x + random < 10 ? crazyLights.at(i)->Radius.x + random : 10;
		random = random > 2 ? random : 2;
		crazyLights.at(i)->Radius.x = random;
		crazyLights.at(i)->Radius.y = random;
		crazyLights.at(i)->Radius.z = random;

		random = (rand() % 2) ? 0.01f : -0.01f;
		random = crazyLights.at(i)->Color.x + random < 1 ? crazyLights.at(i)->Color.x + random : 1;
		random = random > 0 ? random: 0;
		crazyLights.at(i)->Color.x = random;

		random = (rand() % 2) ? 0.01f : -0.01f;
		random = crazyLights.at(i)->Color.y + random < 1 ? crazyLights.at(i)->Color.y + random : 1;
		random = random > 0 ? random : 0;
		crazyLights.at(i)->Color.y = random;

		random = (rand() % 2) ? 0.01f : -0.01f;
		random = crazyLights.at(i)->Color.z + random < 1 ? crazyLights.at(i)->Color.z + random : 1;
		random = random > 0 ? random : 0;
		crazyLights.at(i)->Color.z = random;
	}
}

BrickAuhtScene::~BrickAuhtScene()
{
}

void BrickAuhtScene::Shoot()
{
	if (GetAsyncKeyState(' ') & 0x8000)
	{
		if (spacePressed == false)
		{
			GameEntity* BallEntity = new GameEntity("soccer", "white");
			Ball* ball = new Ball();
			ball->SetEntity(BallEntity);
			ball->kinematics->velocity = *Game::GetCamera()->GetDirection();
			GMath::VectorScale(&ball->kinematics->velocity, 15);
			ball->kinematics->acceleration = VEC3(0, -2.0f, 0);
			ball->kinematics->SetPosition(*Game::GetCamera()->GetPosition());
			ball->entity->SetScale(VEC3(0.1f, 0.1f, 0.1f));
			GameObjects.push_back(ball);
			balls.push_back(ball);
			ammo--;
			ObjectsDirty = true;
		}
		spacePressed = true;
	}
	else
	{
		spacePressed = false;
	}
}