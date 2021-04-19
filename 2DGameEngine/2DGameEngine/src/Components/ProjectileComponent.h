#pragma once

#include <SDL.h>

/*
* \param bool shouldCollideWithPlayer = false
* \param int damagePercentage = 10
* \param  int projectileLifetimeInMs = 5000
* */
struct ProjectileComponent
{
	bool m_shouldCollideWithPlayer;
	int m_damagePercentage;
	int m_projectileLifetimeInMs;
	int m_projectileStartTimeInMs;

	ProjectileComponent(bool shouldCollideWithPlayer = false, int damagePercentage = 10, int projectileLifetimeInMs = 5000)
		: m_shouldCollideWithPlayer(shouldCollideWithPlayer)
		, m_damagePercentage(damagePercentage)
		, m_projectileLifetimeInMs(projectileLifetimeInMs)
		, m_projectileStartTimeInMs(SDL_GetTicks())
	{

	}
};