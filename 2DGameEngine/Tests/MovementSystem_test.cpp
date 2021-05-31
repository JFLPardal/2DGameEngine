#include "pch.h"

#include <glm/glm.hpp>
#include <memory>

#include "ECS/ECS.h"
#include "Game/Game.h"

#include "Systems/MovementSystem.h"
#include "Systems/KeyboardControlSystem.h"
#include "Systems/ProjectileLifeCycleSystem.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/BoxColliderComponent.h"

namespace MovementSystemTests
{
	class MovementSystemSetup : public ::testing::Test
	{
	public:
		MovementSystemSetup()
		{
			m_registry = std::make_unique<Registry>();
			m_registry->AddSystem<MovementSystem>();
			m_registry->AddSystem<ProjectileLifeCycleSystem>();

			m_entity = std::make_unique<Entity>(m_registry->CreateEntity());
			m_entity->AddComponent<TransformComponent>(m_entityInitialPosition);
			m_entity->AddComponent<RigidbodyComponent>(m_entityVelocity);
			m_entity->AddComponent<ProjectileComponent>();
			m_entity->AddComponent<BoxColliderComponent>();

			m_registry->Update();
		}

		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<Entity> m_entity;

		glm::vec2 m_entityInitialPosition{ 0, 0 };
		glm::vec2 m_entityVelocity{ 1, 0 };
	};

	TEST_F(MovementSystemSetup, PositionStaysTheSameWith0Velocity)
	{
		m_entity->GetComponent<RigidbodyComponent>().m_velocity = glm::vec2{ 0, 0 };
		
		double frameDuration = 100;
		m_registry->GetSystem<MovementSystem>().Update(frameDuration);

		auto entityPos = m_entity->GetComponent<TransformComponent>().m_position;

		EXPECT_EQ(entityPos.x, m_entityInitialPosition.x) << "entity's x position is : " << entityPos.x;
		EXPECT_EQ(entityPos.y, m_entityInitialPosition.y) << "entity's y position is : " << entityPos.y;
	}

	TEST_F(MovementSystemSetup, VelocityGT0IfTimeToStopNotReachedAndInitialVelocityGT0)
	{
		const glm::vec2 entityInitialVelocity{ 100, 100 };
		const float entityTimeToStopInSecs = 3.f;
		const float timeToUpdateInSecs = 2.f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;
		
		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<MovementSystem>().Update(timeToUpdateInSecs);
		
		ASSERT_GT(std::abs(entityVelocity.x), 0);
		ASSERT_GT(std::abs(entityVelocity.y), 0);
	}

	TEST_F(MovementSystemSetup, ProjectileVelocity0IfTimeToStopReached)
	{
		const glm::vec2 entityInitialVelocity{ 100, 100 };
		const float entityTimeToStopInSecs = 1.f;
		const float durationOfUpdateInSecs = 1.01f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;

		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<ProjectileLifeCycleSystem>().Update(durationOfUpdateInSecs);

		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.x), 0);
		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.y), 0);
	}

	TEST_F(MovementSystemSetup, ProjectileVelocityGT0IfTimeToStopNotReachedAndInitialVelocityGT0)
	{
		const glm::vec2 entityInitialVelocity{ 100, 100 };
		const float entityTimeToStopInSecs = 2.f;
		const float durationOfUpdateInSecs = 1.5f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;

		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<ProjectileLifeCycleSystem>().Update(durationOfUpdateInSecs);

		ASSERT_GT(std::abs(entityVelocity.x), 0);
		ASSERT_GT(std::abs(entityVelocity.y), 0);
	}
	
	TEST_F(MovementSystemSetup, ProjectileVelocity0IfInitialVelocity0)
	{
		const glm::vec2 entityInitialVelocity{ 0, 0 };
		const float entityTimeToStopInSecs = 2.f;
		const float durationOfUpdateInSecs = 1.f;

		auto& entityRigidbody = m_entity->GetComponent<RigidbodyComponent>();
		auto& entityVelocity = entityRigidbody.m_velocity;

		entityVelocity = entityInitialVelocity;
		entityRigidbody.SetTimeToStopInSecs(entityTimeToStopInSecs);

		m_registry->GetSystem<ProjectileLifeCycleSystem>().Update(durationOfUpdateInSecs);

		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.x), 0);
		ASSERT_DOUBLE_EQ(std::abs(entityVelocity.y), 0);
	}
}