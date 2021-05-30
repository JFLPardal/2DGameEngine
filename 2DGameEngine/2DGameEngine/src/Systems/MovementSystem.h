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

			// 
			//const float velocityThresholdToStop = 5.f;
			//const bool shouldUpdateVelocity = glm::length(rigidbody.m_velocity) > velocityThresholdToStop;
			// 
			// if (shouldUpdateVelocity)
			// {
				// const float baseDecrementPerFrame = 700.f; -> move to rigidbody
				// const auto drag = rigidbody.m_baseDecrementPerFrame / glm::length(rigidbody.m_velocity);
				// 
				// rigidbody.m_velocity -= (rigidbody.m_velocity * drag * deltaTime);
				// transform.m_position += velocity; // does it need * deltaTime?
			//}

			transform.m_position.x += (rigidbody.m_velocity.x * deltaTime);
			transform.m_position.y += (rigidbody.m_velocity.y * deltaTime);

			const bool isPlayer = entity.HasTag("player");
			if (isPlayer)
			{
				int paddingLeft = 10;
				int paddingTop = 10;
				int paddingRight = 50; // 'pivot' of an entity is top left corner
				int paddingBottom = 50;

				transform.m_position.x = transform.m_position.x < paddingLeft ? paddingLeft : transform.m_position.x;
				transform.m_position.x = transform.m_position.x > Game::m_mapWidth - paddingRight ? Game::m_mapWidth - paddingRight : transform.m_position.x;
				transform.m_position.y = transform.m_position.y < paddingTop ? paddingTop : transform.m_position.y;
				transform.m_position.y = transform.m_position.y > Game::m_mapHeight - paddingBottom ? Game::m_mapHeight - paddingBottom : transform.m_position.y;
			}

			// 'ensure' an entity is only destroyed when it is no longer visible
			const int margin = 80;

			const bool isOutOfMap = 
				transform.m_position.x > Game::m_mapWidth + margin ||
				transform.m_position.x  < 0 - margin ||
				transform.m_position.y > Game::m_mapHeight + margin ||
				transform.m_position.y < 0 - margin;

			if (isOutOfMap && !isPlayer)
			{
				entity.Destroy();
			}
		}
	}
};