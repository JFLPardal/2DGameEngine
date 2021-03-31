#pragma once

#include "SDL.h"

#include "ECS/ECS.h"

#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer)
	{
		for (auto entity : GetSystemEntities())
		{
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect objRect = 
			{
				static_cast<int>(transform.m_position.x),
				static_cast<int>(transform.m_position.y),
				static_cast<int>(sprite.m_width),
				static_cast<int>(sprite.m_height)
			};

			SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
			SDL_RenderFillRect(renderer, &objRect);
		}
	}
};