#pragma once

#include "ECS/ECS.h"

struct SpriteComponent
{
	unsigned int m_width;
	unsigned int m_height;

	SpriteComponent(unsigned int width = 5, unsigned int height = 5)
		: m_width(width)
		, m_height(height)
	{
	}
};