#pragma once

#include <SDL.h>

#include "ECS/ECS.h"

#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/BoxColliderComponent.h"

#include "Events/LeftMouseButtonPressedEvent.h"

class ProjectileEmitSystem : public System
{
public:
	ProjectileEmitSystem()
	{
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<LeftMouseButtonPressedEvent>(this, &ProjectileEmitSystem::OnLeftMouseButtonPressed);
	}

	void Update(std::unique_ptr<Registry>& registry)
	{
		for (auto& entity : GetSystemEntities())
		{
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

			const bool shouldEmitProjectile = SDL_GetTicks() - projectileEmitter.m_lastEmissionTimeInMs >=  projectileEmitter.m_frequencyInMs;
			const bool isPlayer = entity.HasTag("player");

			if (shouldEmitProjectile && !isPlayer)
			{
				const auto& transform = entity.GetComponent<TransformComponent>();
				
				auto projectile = registry->CreateEntity();
				projectile.Group("projectiles");
				glm::vec2 projectileSpawnPosition = transform.m_position;

				if (entity.HasComponent<SpriteComponent>())
				{
					const auto& sprite = entity.GetComponent<SpriteComponent>();
					projectileSpawnPosition.x += (transform.m_scale.x * sprite.m_width * .5f);
					projectileSpawnPosition.y += (transform.m_scale.y * sprite.m_height * .5f);
				}

				projectile.AddComponent<TransformComponent>(projectileSpawnPosition);
				projectile.AddComponent<RigidbodyComponent>(projectileEmitter.m_velocity);
				projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
				projectile.AddComponent<BoxColliderComponent>(4, 4);
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_shouldCollideWithPlayer, projectileEmitter.m_damagePercentage, projectileEmitter.m_projectileLifetimeInMs);

				projectileEmitter.m_lastEmissionTimeInMs = SDL_GetTicks();
			}
		}
	}

	void OnLeftMouseButtonPressed(LeftMouseButtonPressedEvent& eventArgs)
	{
		for (auto& entity : GetSystemEntities())
		{
			const bool entityIsPlayer = entity.HasTag("player");
			if (entityIsPlayer)
			{
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const auto& transform = entity.GetComponent<TransformComponent>();
					
				glm::vec2 projectileSpawnPosition = transform.m_position;
				if (entity.HasComponent<SpriteComponent>())
				{
					const auto& sprite = entity.GetComponent<SpriteComponent>();
					projectileSpawnPosition.x += (transform.m_scale.x * sprite.m_width * .5f);
					projectileSpawnPosition.y += (transform.m_scale.y * sprite.m_height * .5f);
				}
				glm::vec2 projectileVelocity;

				auto GetMousePosition = []() -> glm::vec2
				{
					int x = 204, y = 301;
					SDL_GetMouseState(&x, &y);
					return glm::vec2{ x, y };
				};

				const glm::vec2 cameraPosition{ eventArgs.m_cameraPositon.x, eventArgs.m_cameraPositon.y };
				const glm::vec2 mousePositionInScreenCoords{ GetMousePosition() - cameraPosition };

				projectileVelocity = glm::normalize(mousePositionInScreenCoords - projectileSpawnPosition) * projectileEmitter.m_velocity.x;

				auto projectile = entity.m_registry->CreateEntity();
				projectile.Group("projectiles");
				projectile.AddComponent<TransformComponent>(projectileSpawnPosition, glm::vec2(.25f, .25f));
				projectile.AddComponent<RigidbodyComponent>(projectileVelocity);
				projectile.AddComponent<SpriteComponent>("bullet-image", 16, 16, 4);
				projectile.AddComponent<BoxColliderComponent>(4, 4);
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_shouldCollideWithPlayer, projectileEmitter.m_damagePercentage, projectileEmitter.m_projectileLifetimeInMs);
			}
		}
	}
};