#include "BouncingBallScene.h"

BouncingBallScene::BouncingBallScene(Mesh * shape, Material * mat)
{
	for (int i = 0; i < 10; i++)
	{
		GameEntity* firstEntity = new GameEntity(shape, mat);

		Ball* ball = new Ball();
		ball->SetEntity(firstEntity);
		ball->kinematics->velocity = VEC3(0, 0, 0);
		ball->kinematics->acceleration = VEC3(-2.5f + i * 0.5f, -(i + 1), -2.5f + i * 0.5f);
		ball->kinematics->SetPosition(VEC3(-5 + i, 4, 0));
		balls.push_back(ball);
		GameObjects.push_back(ball);
	}
}

void BouncingBallScene::Initialize()
{
	
}

void BouncingBallScene::Update()
{
	// Let's add some logic to make the balls bounce off the walls.
	for (int i = 0; i < balls.size(); i++)
	{
		float boxSize = 5;
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

		if (balls.at(i)->kinematics->GetPosition().z > boxSize)
		{
			balls.at(i)->kinematics->velocity.z = abs(balls.at(i)->kinematics->velocity.z) * -1;
		}
		if (balls.at(i)->kinematics->GetPosition().z < -boxSize)
		{
			balls.at(i)->kinematics->velocity.z = abs(balls.at(i)->kinematics->velocity.z);
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
	}
}
