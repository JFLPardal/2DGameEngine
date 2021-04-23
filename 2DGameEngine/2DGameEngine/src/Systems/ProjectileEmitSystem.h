#pragma once

#include <SDL.h>

#include "ECS/ECS.h"

#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/BoxColliderComponent.h"

#include "Events/KeyPressedEvent.h"

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
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
	}

	void Update(std::unique_ptr<Registry>& registry)
	{
		for (auto& entity : GetSystemEntities())
		{
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

			const bool shouldEmitProjectile = SDL_GetTicks() - projectileEmitter.m_lastEmissionTimeInMs >=  projectileEmitter.m_frequencyInMs;
			if (shouldEmitProjectile && !entity.HasComponent<CameraFollowComponent>())
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

	void OnKeyPressed(KeyPressedEvent& keyPressed)
	{
		if (keyPressed.m_keyPressed == SDLK_SPACE)
		{
			for (auto& entity : GetSystemEntities())
			{
				const bool entityIsPlayer = entity.HasComponent<CameraFollowComponent>();
				if (entityIsPlayer)
				{
					auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
					const auto& transform = entity.GetComponent<TransformComponent>();
					const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
					
					glm::vec2 projectileSpawnPosition = transform.m_position;
					if (entity.HasComponent<SpriteComponent>())
					{
						const auto& sprite = entity.GetComponent<SpriteComponent>();
						projectileSpawnPosition.x += (transform.m_scale.x * sprite.m_width * .5f);
						projectileSpawnPosition.y += (transform.m_scale.y * sprite.m_height * .5f);
					}

					int facingDirectionX = 0;
					int facingDirectionY = 0;
					if (rigidbody.m_velocity.x > 0) facingDirectionX = +1;
					if (rigidbody.m_velocity.x < 0) facingDirectionX = -1;
					if (rigidbody.m_velocity.y > 0) facingDirectionY = +1;
					if (rigidbody.m_velocity.y < 0) facingDirectionY = -1;

					glm::vec2 projectileVelocity;
					projectileVelocity.x = projectileEmitter.m_velocity.x * facingDirectionX;
					projectileVelocity.y = projectileEmitter.m_velocity.y * facingDirectionY;

					auto projectile = entity.m_registry->CreateEntity();
					projectile.Group("projectiles");
					projectile.AddComponent<TransformComponent>(projectileSpawnPosition);
					projectile.AddComponent<RigidbodyComponent>(projectileVelocity);
					projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
					projectile.AddComponent<BoxColliderComponent>(4, 4);
					projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_shouldCollideWithPlayer, projectileEmitter.m_damagePercentage, projectileEmitter.m_projectileLifetimeInMs);
				}
			}
		}
	}
};