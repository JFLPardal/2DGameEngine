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

	//void Update(double deltaTime)
	//{
	//	for (auto& entity : GetSystemEntities())
	//	{
	//		auto& projectile = entity.GetComponent<ProjectileComponent>();
	//		auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
	//			
	//		const float velocityThresholdToStop = 5.f;

	//		// this behaviour should be on MovementSystem as well
	//		const bool shouldUpdateVelocity = glm::length(rigidbody.m_velocity) > velocityThresholdToStop;
	//		if (shouldUpdateVelocity)
	//		{
	//			// this should be done by the MovementSystem. In this system we are only interested in keeping the 'else' branch
	//			const float movementResistanceFactor = rigidbody.m_movementResistanceFactor;

	//			const auto velocityDecrementPerFrame = movementResistanceFactor / glm::length(rigidbody.m_velocity);
	//			rigidbody.m_velocity -= (rigidbody.m_velocity * static_cast<float>(velocityDecrementPerFrame * deltaTime));
	//		}
	//		else
	//		{
	//			rigidbody.m_velocity = { 0,0 }; // remove this as well
	//			entity.GetComponent<BoxColliderComponent>().m_isActive = false;
	//		}
	//	}
	//}

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

				// this behaviour should be on MovementSystem as well
				const double timeRemainingToUpdate = (SDL_GetTicks() - entitySpawnTime ) * .001f + deltaTime;
				const bool shouldUpdateVelocity = timeToStopInSecs > timeRemainingToUpdate;
				if (shouldUpdateVelocity)
				{
					// this should be done by the MovementSystem. In this system we are only interested in keeping the 'else' branch
					//const float movementResistanceFactor = rigidbody.m_movementResistanceFactor;

					rigidbody.m_velocity -= rigidbody.GetVelocityDecrement() * static_cast<float>(deltaTime);
					
					const float minimumVelocityMagnitudeToKeepUpdating = 2.f;
					const bool isStopped = glm::length(rigidbody.m_velocity) < minimumVelocityMagnitudeToKeepUpdating;
					if (isStopped)
					{
						rigidbody.m_velocity = { 0,0 };
						entity.GetComponent<BoxColliderComponent>().m_isActive = false;
					}
				}
				else
				{
					rigidbody.m_velocity = { 0,0 }; // remove this as well
					entity.GetComponent<BoxColliderComponent>().m_isActive = false;
				}
			}
		}
	}
};