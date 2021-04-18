#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

/*
* \param glm::vec2 velocity = glm::vec2{ 10,10 }
* \param  int frequencyInMs = 1000
* \param int projectileLifetimeInMs = 6000
* \param int damagePercentage = 10
* \param bool shouldCollideWithPlayer = false
* */
struct ProjectileEmitterComponent
{
	glm::vec2 m_velocity;
	int m_frequencyInMs;
	int m_projectileLifetimeInMs;
	int m_damagePercentage; // percentage of life the projectile should take away 
	bool m_shouldCollideWithPlayer;
	int m_lastEmissionTimeInMs;

	ProjectileEmitterComponent(glm::vec2 velocity = glm::vec2{ 10,10 }, int frequencyInMs = 1000,
	int projectileLifetimeInMs = 6000, int damagePercentage = 10, bool shouldCollideWithPlayer = false)
		: m_velocity(velocity)
		, m_frequencyInMs(frequencyInMs)
		, m_projectileLifetimeInMs(projectileLifetimeInMs)
		, m_damagePercentage(damagePercentage)
		, m_shouldCollideWithPlayer(shouldCollideWithPlayer)
		, m_lastEmissionTimeInMs(SDL_GetTicks())
	{
		if (damagePercentage >= 1 && damagePercentage <= 100)
		{
			m_damagePercentage = damagePercentage;
		}
		else
		{
			Logger::Error("ProjectileEmitterComponent: m_damagePercentage not in the range [1, 100]");
		}
	}
};