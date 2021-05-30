#include "pch.h"

#include <glm/glm.hpp>
#include <memory>

#include "ECS/ECS.h"
#include "Game/Game.h"

#include "Systems/MovementSystem.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"

#include "Systems/KeyboardControlSystem.h"


class MockClass
{
public:
	MOCK_METHOD(void, MethodThatWasMocked, (int));

};
TEST(MockClassTesting, CanCreateMock)
{
	Game game;
	MockClass mockClass;
	EXPECT_CALL(mockClass, MethodThatWasMocked);

	mockClass.MethodThatWasMocked(5);
}

class KeyboardControlSystemFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		registry.AddSystem<KeyboardControlSystem>();
	}
	Registry registry{};
};
TEST_F(KeyboardControlSystemFixture, TestingIfUpdateDoesntBreakTestFramework)
{
	registry.GetSystem<KeyboardControlSystem>().Update(1.0);
	ASSERT_EQ(0, 0);
}

namespace MovementSystemTests
{
	class MovementSystemSetup : public ::testing::Test
	{
	public:
		MovementSystemSetup()
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
		Game g;
		auto sa = Game::m_windowHeight;
		//g.Initialize();
		auto entityExpectedPos = helpers::GetExpectedPos(
			m_entityInitialPosition,
			m_entityVelocity,
			frameDuration);
		auto entityPos = m_entity->GetComponent<TransformComponent>().m_position;

		EXPECT_EQ(entityPos.x, sa) << "entity's x position is : " << entityPos.x;
		EXPECT_EQ(entityPos.x, entityExpectedPos.x) << "entity's x position is : " << entityPos.x;
		EXPECT_EQ(entityPos.y, entityExpectedPos.y) << "entity's y position is : " << entityPos.y;
	}

	TEST_F(MovementSystemSetup, PositionDoesNotUpdateWith0SpeedAndNon0Time22)
	{
		m_entity->GetComponent<RigidbodyComponent>().m_velocity = glm::vec2{ 0, 0 };
		
		double frameDuration = 100.9;
		m_registry->GetSystem<MovementSystem>().Update(frameDuration); //-> problem is the Game::m_mapWidth called in this function it seems

		auto entityPos = m_entity->GetComponent<TransformComponent>().m_position;

		EXPECT_EQ(entityPos.x, m_entityInitialPosition.x);// << "entity's x position is : " << entityPos.x;
		EXPECT_EQ(entityPos.y, m_entityInitialPosition.y);// << "entity's y position is : " << entityPos.y;
	}
}