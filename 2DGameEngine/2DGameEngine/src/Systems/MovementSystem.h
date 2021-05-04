#pragma once

#include "Game/Game.h"

#include "Events/CollisionEvent.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
	}

	void OnCollision(CollisionEvent& eventParams)
	{
		Entity& a = eventParams.m_a;
		Entity& b = eventParams.m_b;

		// enemy and obstacle collision
		if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles"))
		{
			OnEnemyCollidesObstacle(a, b);
		}
		if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles"))
		{
			OnEnemyCollidesObstacle(b, a);
		}
	}

	void OnEnemyCollidesObstacle(Entity& enemy, Entity& obstacle)
	{
		if (enemy.HasComponent<RigidbodyComponent>() && enemy.HasComponent<SpriteComponent>())
		{
			auto& rigidbody = enemy.GetComponent<RigidbodyComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();

			if (rigidbody.m_velocity.x != 0)
			{
				rigidbody.m_velocity.x *= -1;
				sprite.m_flip = (sprite.m_flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}
			if (rigidbody.m_velocity.y != 0)
			{
				rigidbody.m_velocity.y *= -1; 
				sprite.m_flip = (sprite.m_flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
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