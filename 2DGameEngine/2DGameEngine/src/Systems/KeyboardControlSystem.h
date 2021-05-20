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
		//eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
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

	void Update()
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
			auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

			auto keyboardStateArray = SDL_GetKeyboardState(NULL);

			bool isMovingInX = false;
			bool isMovingInY = false;
			glm::vec2 newVelocity = rigidbody.m_velocity;

			if (keyboardStateArray[SDL_SCANCODE_W])
			{
				newVelocity.y = keyboardControl.m_upVelocity.y;
				isMovingInY = true;
			}
			if (keyboardStateArray[SDL_SCANCODE_A])
			{
				newVelocity.x = keyboardControl.m_leftVelocity.x;
				isMovingInX = true;
			}
			if (keyboardStateArray[SDL_SCANCODE_S])
			{
				newVelocity.y = keyboardControl.m_downVelocity.y;
				isMovingInY = true;
			}
			if (keyboardStateArray[SDL_SCANCODE_D])
			{
				newVelocity.x = keyboardControl.m_rightVelocity.x;
				isMovingInX = true;
			}
			// TODO normalize velocity for diagonal speed
			// TODO cleanup/simplify the code, this is messy as hell
			if (!isMovingInX)
			{
				const bool isMovingRight = rigidbody.m_velocity.x > 0;
				const auto velocityDecrementInX = 
					(isMovingRight) ?
					static_cast<int>(rigidbody.m_velocity.x - keyboardControl.m_rightVelocity.x * .01f) :
					static_cast<int>(rigidbody.m_velocity.x + keyboardControl.m_rightVelocity.x * .01f);

				const int maxVelocity = keyboardControl.m_rightVelocity.x;
				newVelocity.x = 
					(isMovingRight) ?
					glm::clamp(velocityDecrementInX, 0, maxVelocity) :
					glm::clamp(velocityDecrementInX, -maxVelocity, 0);
			}
			if (!isMovingInY)
			{
				const bool isMovingUp = rigidbody.m_velocity.y < 0;
				const auto velocityDecrementInY = 
					(isMovingUp) ?
					static_cast<int>(rigidbody.m_velocity.y - keyboardControl.m_upVelocity.y * .01f) :
					static_cast<int>(rigidbody.m_velocity.y + keyboardControl.m_upVelocity.y * .01f);

				const int maxVelocity = keyboardControl.m_upVelocity.y;
				newVelocity.y = 
					(isMovingUp) ?
					glm::clamp(velocityDecrementInY, maxVelocity, 0) :
					glm::clamp(velocityDecrementInY, 0, -maxVelocity)
					;

				Logger::Error(std::to_string(newVelocity.y));
			}
			rigidbody.m_velocity = newVelocity;
		}
	}
};