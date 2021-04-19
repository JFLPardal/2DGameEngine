#pragma once

#include "ECS/ECS.h"

#include "Components/ProjectileComponent.h"

class ProjectileLifeCycleSystem : public System
{
public:
	ProjectileLifeCycleSystem()
	{
		RequireComponent<ProjectileComponent>();
	}

	void Update()
	{
		for (auto& entity : GetSystemEntities())
		{
			auto& projectile = entity.GetComponent<ProjectileComponent>();

			const bool shouldBeDestroyed = SDL_GetTicks() - projectile.m_projectileStartTimeInMs >= projectile.m_projectileLifetimeInMs;
			if (shouldBeDestroyed)
			{
				entity.Destroy();
			}
		}
	}
};