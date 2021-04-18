#include "pch.h"

#include <gtest/gtest.h>

#include "ECS/ECS.h"

#include "Systems/MovementSystem.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"

namespace MovementSystemTests
{
	class MovementSystemSetup : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			m_registry = std::make_unique<Registry>();
			m_registry->AddSystem<MovementSystem>();

			m_entity = std::make_unique<Entity>(m_registry->CreateEntity());
			m_entity->AddComponent<TransformComponent>(m_entityInitialPosition);
			m_entity->AddComponent<RigidbodyComponent>(m_entityVelocity);

			m_registry->Update();
		}

		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<Entity> m_entity;

		glm::vec2 m_entityInitialPosition{ 0, 0 };
		glm::vec2 m_entityVelocity{ 1, 0 };
	};

	namespace helpers
	{
		glm::vec2 GetExpectedPos(const glm::vec2& initialPos, const glm::vec2& velocity, double frameDuration)
		{
			glm::vec2 expectedPos
			{
				initialPos.x + (velocity.x * frameDuration),
				initialPos.y + (velocity.y * frameDuration),
			};
			return expectedPos;
		}
	}

	TEST_F(MovementSystemSetup, PositionUpdatesWithNon0SpeedAndNon0Time)
	{
		double frameDuration = 100;
		m_registry->GetSystem<MovementSystem>().Update(frameDuration);

		auto entityExpectedPos = helpers::GetExpectedPos(
			m_entityInitialPosition, 
			m_entityVelocity, 
			frameDuration);
		auto entityPos = m_entity->GetComponent<TransformComponent>().m_position;

		EXPECT_EQ(entityPos.x, entityExpectedPos.x) << "entity's x position is : " << entityPos.x;
		EXPECT_EQ(entityPos.y, entityExpectedPos.y) << "entity's y position is : " << entityPos.y;
	}

	TEST_F(MovementSystemSetup, PositionDoesNotUpdateWith0SpeedAndNon0Time)
	{
		m_entity->GetComponent<RigidbodyComponent>().m_velocity = glm::vec2{ 0, 0 };

		double frameDuration = 100;
		m_registry->GetSystem<MovementSystem>().Update(frameDuration);

		auto entityPos = m_entity->GetComponent<TransformComponent>().m_position;

		EXPECT_EQ(entityPos.x, m_entityInitialPosition.x) << "entity's x position is : " << entityPos.x;
		EXPECT_EQ(entityPos.y, m_entityInitialPosition.y) << "entity's y position is : " << entityPos.y;
	}
}