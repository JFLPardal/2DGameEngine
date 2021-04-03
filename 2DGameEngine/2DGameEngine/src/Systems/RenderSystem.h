#pragma once

#include "SDL.h"

#include "ECS/ECS.h"
#include "AssetStore/AssetStore.h"

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

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
	{
		for (auto entity : GetSystemEntities())
		{
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect srcRect = sprite.m_textureRect;

			SDL_Rect dstRect = 
			{
				static_cast<int>(transform.m_position.x),
				static_cast<int>(transform.m_position.y),
				static_cast<int>(sprite.m_width * transform.m_scale.x),
				static_cast<int>(sprite.m_height * transform.m_scale.y)
			};

			SDL_RenderCopyEx(
				renderer,
				assetStore->GetTexture(sprite.m_assetId),
				&srcRect,
				&dstRect,
				transform.m_rotation,
				NULL,
				SDL_FLIP_NONE
			);
		}
	}
};