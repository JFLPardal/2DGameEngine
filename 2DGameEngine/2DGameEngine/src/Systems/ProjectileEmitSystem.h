#pragma once

#include <SDL.h>

#include "ECS/ECS.h"

#include "Components/ProjectileEmitterComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/BoxColliderComponent.h"


class ProjectileEmitSystem : public System
{
public:
	ProjectileEmitSystem()
	{
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(std::unique_ptr<Registry>& registry)
	{
		for (auto& entity : GetSystemEntities())
		{
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto& transform = entity.GetComponent<TransformComponent>();

			const bool shouldEmitProjectile = SDL_GetTicks() - projectileEmitter.m_lastEmissionTimeInMs >=  projectileEmitter.m_frequencyInMs;
			if (shouldEmitProjectile)
			{
				auto projectile = registry->CreateEntity();
				glm::vec2 projectileSpawnPosition = transform.m_position;
				
				if (entity.HasComponent<SpriteComponent>())
				{
					const auto& sprite = entity.GetComponent<SpriteComponent>();
					projectileSpawnPosition.x += (transform.m_scale.x *  sprite.m_width * .5f);
					projectileSpawnPosition.y += (transform.m_scale.y * sprite.m_height * .5f);
				}
				projectile.AddComponent<TransformComponent>(projectileSpawnPosition);
				projectile.AddComponent<RigidbodyComponent>(projectileEmitter.m_velocity);
				projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
				projectile.AddComponent<BoxColliderComponent>(4, 4);

				projectileEmitter.m_lastEmissionTimeInMs = SDL_GetTicks();
			}
		}
	}
};