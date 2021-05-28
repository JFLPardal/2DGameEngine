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
			auto& projectile = entity.GetComponent<ProjectileComponent>();
			auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
				
			const float velocityThresholdToStop = 5.f;

			const bool shouldUpdateVelocity = glm::length(rigidbody.m_velocity) > velocityThresholdToStop;
			if (shouldUpdateVelocity)
			{
				const float baseDecrementPerFrame = 700.f;

				const auto velocityDecrementPerFrame = baseDecrementPerFrame / glm::length(rigidbody.m_velocity);
				rigidbody.m_velocity -= (rigidbody.m_velocity * static_cast<float>(velocityDecrementPerFrame * deltaTime));
			}
			else
			{
				rigidbody.m_velocity = { 0,0 };
				entity.GetComponent<BoxColliderComponent>().m_isActive = false;
			}
		}
	}
};