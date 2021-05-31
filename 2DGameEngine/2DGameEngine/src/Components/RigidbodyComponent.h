#pragma once

#include "glm/glm.hpp"

/*
* \param glm::vec2 velocity = (0,0)
* \param float timeToStopInSecs = .5f - seconds it takes for the entity to come to a full stop after being pushed
*/
struct RigidbodyComponent
{
	glm::vec2 m_velocity;
	glm::vec2 m_initialVelocity;

	RigidbodyComponent(glm::vec2 velocity = glm::vec2(0, 0), float timeToStopInSecs = .5f)
		: m_velocity(velocity)
		, m_initialVelocity(velocity)
	{
		SetTimeToStopInSecs(timeToStopInSecs);
	}

	inline float GetTimeToStopInSecs() const { return m_timeToStopInSecs; }
	inline void SetTimeToStopInSecs(float timeToStopInSecs) { m_timeToStopInSecs = (timeToStopInSecs > 0) ? timeToStopInSecs : 0.1f; CalculateVelocityDecrement(); }

	const glm::vec2& GetVelocityDecrement() const { return m_velocityDecrement; }
private:
	inline void CalculateVelocityDecrement() { m_velocityDecrement = (m_initialVelocity / m_timeToStopInSecs); }

	float m_timeToStopInSecs;
	glm::vec2 m_velocityDecrement;
};