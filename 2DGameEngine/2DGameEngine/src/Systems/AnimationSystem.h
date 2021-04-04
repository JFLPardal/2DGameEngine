#pragma once

#include <SDL.h>

#include "ECS/ECS.h"

#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"

class AnimationSystem : public System
{
public:
	AnimationSystem()
	{
		RequireComponent<AnimationComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update()
	{
		for (auto& entity : GetSystemEntities())
		{
			auto& animation = entity.GetComponent<AnimationComponent>();
			auto& sprite = entity.GetComponent<SpriteComponent>();

			animation.m_currentFrame = (	(SDL_GetTicks() - animation.m_startTime)
				* animation.m_frameSpeedRate / 1000) % animation.m_numFrames;

			sprite.m_textureRect.x = animation.m_currentFrame * sprite.m_width;
		}
	}
};