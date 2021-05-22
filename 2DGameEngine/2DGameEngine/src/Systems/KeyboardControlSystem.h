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

			const auto maxHorizontalVelocity = keyboardControl.m_rightVelocity.x;
			const auto maxVerticalVelocity = keyboardControl.m_upVelocity.y;

			glm::vec2 newVelocity{ rigidbody.m_velocity };

			//calculate the normalized direction of the movement
			glm::vec2 velocityDirection = { 0, 0 };
			{
				if (keyboardStateArray[SDL_SCANCODE_W]) velocityDirection.y -= maxVerticalVelocity;
				if (keyboardStateArray[SDL_SCANCODE_A]) velocityDirection.x -= maxHorizontalVelocity;
				if (keyboardStateArray[SDL_SCANCODE_S]) velocityDirection.y += maxVerticalVelocity;
				if (keyboardStateArray[SDL_SCANCODE_D])	velocityDirection.x += maxHorizontalVelocity;

				isMovingInX = velocityDirection.x != 0;
				isMovingInY = velocityDirection.y != 0;

				if (isMovingInX || isMovingInY)
				{
					velocityDirection = glm::normalize(velocityDirection);
				}
			}

			// calculate the magnitude of the movement
			{
				glm::vec2 velocityMagnitude{ 0, 0 };
				const auto currentVelocity{ rigidbody.m_velocity };

				if (isMovingInX)
				{
					velocityMagnitude.x = maxHorizontalVelocity;
				}
				else
				{
					const auto velocityDecrementInX = currentVelocity.x * .02f;
					velocityMagnitude.x = static_cast<int>(currentVelocity.x - velocityDecrementInX);
				}

				if (isMovingInY)
				{
					velocityMagnitude.y = maxVerticalVelocity;
				}
				else
				{
					const auto velocityDecrementInY = currentVelocity.y * .02f;
					velocityMagnitude.y = static_cast<int>(currentVelocity.y - velocityDecrementInY);
				}

				if (isMovingInX || isMovingInY)
				{
					newVelocity = velocityDirection * velocityMagnitude;
				}
				else
				{
					newVelocity = velocityMagnitude;
				}
			}

			rigidbody.m_velocity = newVelocity;
		}
	}
};