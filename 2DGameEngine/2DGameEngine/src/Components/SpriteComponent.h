#pragma once

#include <string>
#include<SDL.h>

#include "ECS/ECS.h"

struct SpriteComponent
{
	std::string m_assetId;
	unsigned int m_width;
	unsigned int m_height;
	// this rect allows the texture to come from a tilemaps 
	SDL_Rect m_textureRect;
	

	SpriteComponent(const std::string& assetId = "", unsigned int width = 5, unsigned int height = 5, Uint8 tilemapXCoord = 0, Uint8 tilemapYCoord = 0)
		: m_assetId(assetId)
		, m_width(width)
		, m_height(height)
		, m_textureRect(SDL_Rect{
			static_cast<int>(tilemapXCoord), 
			static_cast<int>(tilemapYCoord), 
			static_cast<int>(width),
			static_cast<int>(height)
			})
	{
	}
};