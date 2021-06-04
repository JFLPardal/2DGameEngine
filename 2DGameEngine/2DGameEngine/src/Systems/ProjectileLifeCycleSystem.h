#pragma once

#include "ECS/ECS.h"

#include "Components/ProjectileComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/BoxColliderComponent.h"

class ProjectileLifeCycleSystem : public System
{
public:
	ProjectileLifeCycleSystem()
	{
		RequireComponent<ProjectileComponent>();
		RequireComponent<RigidbodyComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(double deltaTime)
	{
		for (auto& entity : GetSystemEntities())
		{
			if (entity.GetComponent<BoxColliderComponent>().m_isActive)
			{
				auto& projectile = entity.GetComponent<ProjectileComponent>();
				auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

				const float timeToStopInSecs = rigidbody.GetTimeToStopInSecs();
				const double entitySpawnTime = projectile.m_projectileStartTimeInMs;

				const double timeRemainingToUpdateInSecs = (SDL_GetTicks() - entitySpawnTime ) * .001f + deltaTime;
				const bool shouldUpdateVelocity = timeToStopInSecs > timeRemainingToUpdateInSecs;
				if (shouldUpdateVelocity)
				{
					rigidbody.m_velocity -= rigidbody.GetVelocityDecrement() * static_cast<float>(deltaTime);
					
					{
						const bool shouldStop = glm::length(rigidbody.m_velocity) < minimumVelocityMagnitudeToKeepUpdating;
						if (shouldStop)
						{
							StopEntity(entity);
						}
					}
				}
				else
				{
					StopEntity(entity);
				}
			}
		}
	}
private:
	void StopEntity(Entity& entityToStop)
	{
		entityToStop.GetComponent<RigidbodyComponent>().m_velocity = { 0,0 };
		entityToStop.GetComponent<BoxColliderComponent>().m_isActive = false;
	}
};