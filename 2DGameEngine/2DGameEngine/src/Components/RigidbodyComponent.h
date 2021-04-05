#pragma once

#include "glm/glm.hpp"

/*
* \param glm::vec2 velocity = (0,0)
*/
struct RigidbodyComponent
{
	glm::vec2 m_velocity;

	RigidbodyComponent(glm::vec2 velocity = glm::vec2(0, 0))
		:m_velocity(velocity)
	{
	}
};