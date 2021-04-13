#pragma once

#include "ECS/ECS.h"
#include "EventBus/EventBus.h"

#include "Events/KeyPressedEvent.h"

#include "Components/KeyboardControlledComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"

class KeyboardControlSystem : public System
{
	// these values need to be coherent with the 
	// way the sprites are aligned on the sheet
	enum class Direction : size_t
	{
		up = 0,
		right,
		down,
		left
	};

public:
	KeyboardControlSystem()
	{
		RequireComponent<KeyboardControlledComponent>();
		RequireComponent<RigidbodyComponent>();
		RequireComponent<SpriteComponent>();
	}
	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& keyPressedEvent)
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
			auto& sprite = entity.GetComponent<SpriteComponent>();
			auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
			
			auto SetVelocityAndTextureRect = [&sprite, &rigidbody](glm::vec2 velocity, Direction ySpriteIndex)
			{
				rigidbody.m_velocity = velocity;
				sprite.m_textureRect.y = sprite.m_height * static_cast<int>(ySpriteIndex);
			};

			switch (keyPressedEvent.m_keyPressed)
			{
			case(SDL_KeyCode::SDLK_w):
				SetVelocityAndTextureRect(keyboardControl.m_upVelocity, Direction::up);
				break;
			case(SDL_KeyCode::SDLK_d):
				SetVelocityAndTextureRect(keyboardControl.m_rightVelocity, Direction::right);
				break;
			case(SDL_KeyCode::SDLK_s):
				SetVelocityAndTextureRect(keyboardControl.m_downVelocity, Direction::down);
				break;
			case(SDL_KeyCode::SDLK_a):
				SetVelocityAndTextureRect(keyboardControl.m_leftVelocity, Direction::left);
				break;
			}

		}
	}
};