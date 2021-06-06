#include "pch.h"

#include "ECS/ECS.h"
#include "Game/Game.h"

#include "Systems/MovementSystem.h"

namespace MovementSystemTests
{
	class EnemiesAndProjectilesMovementSetup : public ::testing::Test
	{
	public:
		EnemiesAndProjectilesMovementSetup()
		{
			m_registry = std::make_unique<Registry>();
			m_registry->AddSystem<MovementSystem>();

			m_entity = std::make_unique<Entity>(m_registry->CreateEntity());
			m_entity->AddComponent<TransformComponent>(m_entityInitialPosition);
			m_entity->AddComponent<RigidbodyComponent>(m_entityVelocity);
			m_entity->AddComponent<BoxColliderComponent>();

			m_registry->Update();
		}

		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<Entity> m_entity;

		glm::vec2 m_entityInitialPosition{ 0, 0 };
		glm::vec2 m_entityVelocity{ 1, 0 };
	};

	// this should be an entity test, not a projectile
	TEST_F(EnemiesAndProjectilesMovementSetup, Given0Velocity_ThenPositionStaysTheSame)
	{
		m_entity->GetComponent<RigidbodyComponent>().m_velocity = glm::vec2{ 0, 0 };
		
		double durationOfUpdateInSecs = 100;
		m_registry->GetSystem<MovementSystem>().Update(durationOfUpdateInSecs);

		auto entityPos = m_entity->GetComponent<TransformComponent>().m_position;

		EXPECT_FLOAT_EQ(entityPos.x, m_entityInitialPosition.x);
		EXPECT_FLOAT_EQ(entityPos.y, m_entityInitialPosition.y);
	}

	TEST_F(EnemiesAndProjectilesMovementSetup, Given0Velocity_WhenTimeToStopNotReached_ThenVelocityIs0AndPositionStaysTheSame)
	{
		const glm::vec2 entityInitialVelocity{ 0, 0 };
		const float entityTimeToStopInSecs = 2.f;
		const float durationOfUpdateInSecs = 1.f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;

		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<MovementSystem>().Update(durationOfUpdateInSecs);
		
		const auto& entityPosition = m_entity->GetComponent<TransformComponent>().m_position;

		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.x), 0);
		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.y), 0);
		ASSERT_DOUBLE_EQ(std::abs(entityPosition.x), m_entityInitialPosition.x);
		ASSERT_DOUBLE_EQ(std::abs(entityPosition.y), m_entityInitialPosition.y);
	}

	TEST_F(EnemiesAndProjectilesMovementSetup, GivenNon0Velocity_WhenTimeToStopNotReached_ThenVelocityGT0)
	{
		const glm::vec2 entityInitialVelocity{ 100, 100 };
		const float entityTimeToStopInSecs = 3.f;
		const float durationOfUpdateInSecs = 2.f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;
		
		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<MovementSystem>().Update(durationOfUpdateInSecs);
		
		ASSERT_GT(std::abs(entityVelocity.x), 0);
		ASSERT_GT(std::abs(entityVelocity.y), 0);
	}

	TEST_F(EnemiesAndProjectilesMovementSetup, GivenNon0Velocity_WhenTimeToStopReached_ThenVelocityIs0)
	{
		const glm::vec2 entityInitialVelocity{ 100, 100 };
		const float entityTimeToStopInSecs = 1.f;
		const float durationOfUpdateInSecs = 1.1f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;

		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<MovementSystem>().Update(durationOfUpdateInSecs);

		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.x), 0);
		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.y), 0);
	}
}