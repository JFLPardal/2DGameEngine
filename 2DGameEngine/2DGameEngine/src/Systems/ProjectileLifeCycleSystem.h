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
				
				if (entity.GetComponent<RigidbodyComponent>().IsStopped())
				{
					entity.GetComponent<BoxColliderComponent>().m_isActive = false;
				}
			}
		}
	}
};