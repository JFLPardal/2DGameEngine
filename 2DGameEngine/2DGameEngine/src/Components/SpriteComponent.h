#pragma once

#include "ECS/ECS.h"
#include <string>

struct SpriteComponent
{
	std::string m_assetId;
	unsigned int m_width;
	unsigned int m_height;

	SpriteComponent(const std::string& assetId = "", unsigned int width = 5, unsigned int height = 5)
		: m_assetId(assetId)
		, m_width(width)
		, m_height(height)
	{
	}
};