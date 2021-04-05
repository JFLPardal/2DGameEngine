#pragma once

#include "ECS/ECS.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
	}

	void Update(double deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

			transform.m_position.x += (rigidbody.m_velocity.x * deltaTime);
			transform.m_position.y += (rigidbody.m_velocity.y * deltaTime);

			/*Logger::Log("Entity id = " + std::to_string(entity.GetId())
				+ " position: [" + std::to_string(transform.m_position.x) 
			+ ", " + std::to_string(transform.m_position.y) + "]");*/	
		}
	}
};