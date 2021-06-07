#pragma once

#include "ECS/ECS.h"

#include "EventBus/EventBus.h"
#include "Events/CollisionEvent.h"

#include "Components/BoxColliderComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/DummyCharacterComponent.h"

class DamageSystem : public System
{
public:
	DamageSystem()
	{
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);

	}

	void OnCollision(CollisionEvent& eventParams)
	{
		Entity& a = eventParams.m_a;
		Entity& b = eventParams.m_b;
		
		//Logger::Log("CollisionEvent between: " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));
		
		// projectile and player collisions
		if (a.BelongsToGroup("projectiles") && b.HasTag("player"))
		{
			OnProjectileCollidesPlayer(a, b);
		}
		if (b.BelongsToGroup("projectiles") && a.HasTag("player"))
		{
			OnProjectileCollidesPlayer(b, a);
		}

		// projectile and enemies collisions
		if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies"))
		{
			OnProjectileCollidesEnemy(b, a);
		}
		if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies"))
		{
			OnProjectileCollidesEnemy(a, b);
		}
	}

private:
	void OnProjectileCollidesPlayer(Entity& projectile, Entity& player)
	{
		const auto& projectileComponent = projectile.GetComponent<ProjectileComponent>();
		
		if (projectileComponent.m_shouldCollideWithPlayer)
		{
			auto& playerHealth = player.GetComponent<HealthComponent>();

			playerHealth.m_currentHealthPertcentage -= projectileComponent.m_damagePercentage;

			if (playerHealth.m_currentHealthPertcentage <= 0)
			{
				player.Destroy();
			}

			projectile.Destroy();
		}
	}

	void OnProjectileCollidesEnemy(Entity& projectile, Entity& enemy)
	{
		const auto& projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (enemy.HasComponent<RigidbodyComponent>())
		{
			const auto& projectileRigidbody = projectile.GetComponent<RigidbodyComponent>();
			auto& enemyRigidbody = enemy.GetComponent<RigidbodyComponent>();

			const float velocityPenaltyWhenHitInPercentage = .3f;
			enemyRigidbody.m_velocity += (projectileRigidbody.m_velocity * velocityPenaltyWhenHitInPercentage);
			enemyRigidbody.WasPushed();

			if (!projectileComponent.m_shouldCollideWithPlayer && enemy.HasComponent<HealthComponent>() )
			{
				auto& enemyHealth = enemy.GetComponent<HealthComponent>();

				enemyHealth.m_currentHealthPertcentage -= projectileComponent.m_damagePercentage;

				if (enemyHealth.m_currentHealthPertcentage <= 0)
				{
					enemy.Destroy();
				}

				projectile.Destroy();
			}
			else if(enemy.HasComponent<DummuCharacterComponent>())
			{
				projectile.Destroy();
			}
		}
		else
		{
			Logger::Log("collision between a projectile and an enemy that doesn't have Rigidbody, intended?");
		}
	}
};