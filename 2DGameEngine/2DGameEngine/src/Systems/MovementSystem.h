#pragma once

//#include "ECS/ECS.h"
#include "Game/Game.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"


class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
	}

	void Update(double deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

			transform.m_position.x += (rigidbody.m_velocity.x * deltaTime);
			transform.m_position.y += (rigidbody.m_velocity.y * deltaTime);

			// 'ensure' an entity is only destroyed when it is no longer visible
			const int margin = 100;

			const bool isOutOfMap = 
				transform.m_position.x > Game::m_mapWidth + margin ||
				transform.m_position.x  < 0 - margin ||
				transform.m_position.y > Game::m_mapHeight + margin ||
				transform.m_position.y < 0 - margin;

			const bool isPlayer = entity.HasTag("player");
			if (isOutOfMap && !isPlayer)
			{
				entity.Destroy();
			}
		}
	}
};