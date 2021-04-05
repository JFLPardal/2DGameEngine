#pragma once

#include <glm/glm.hpp>

/*
* \param uint width = 32
* \param uint height = 32
* \param glm::vec2 offset = (0,0)
*/
struct BoxColliderComponent
{
	unsigned int m_width;
	unsigned int m_height;
	glm::vec2 m_offset;

	BoxColliderComponent(unsigned int width = 32, unsigned int height = 32, glm::vec2 offset = glm::vec2(0,0))
		: m_width(width)
		, m_height(height)
		, m_offset(offset)
	{
	}
};