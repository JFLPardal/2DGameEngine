#pragma once

#include <algorithm>

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
		// sort all the entities by their z-index
		struct RenderableEntity
		{
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};

		std::vector<RenderableEntity> renderableEntities;
		for (const auto& entity : GetSystemEntities())
		{
			RenderableEntity renderableEntity;
			renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
			renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
			renderableEntities.emplace_back(renderableEntity);
		}

		std::sort(renderableEntities.begin(), renderableEntities.end(),
			[](const RenderableEntity& first, const RenderableEntity& second)
			{
				return first.spriteComponent.m_zIndex < second.spriteComponent.m_zIndex;
			}
		);

		for (auto entity : renderableEntities)
		{
			const auto transform = entity.transformComponent;
			const auto sprite = entity.spriteComponent;

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