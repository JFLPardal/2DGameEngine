#include "pch.h"

#include "ECS/ECS.h"
#include "Game/Game.h"

#include "Systems/MovementSystem.h"
#include "Systems/ProjectileLifeCycleSystem.h"
#include "Systems/ProjectileEmitSystem.h"

namespace PlayerProjectileFiring
{
	class PlayerProjectileFiringWithInfiniteFireRateSetup : public ::testing::Test
	{
	public:
		PlayerProjectileFiringWithInfiniteFireRateSetup()
		{
			//setup registry
			m_registry = std::make_unique<Registry>();
			m_registry->AddSystem<MovementSystem>();
			m_registry->AddSystem<ProjectileEmitSystem>();
			m_registry->AddSystem<ProjectileLifeCycleSystem>();

			// setup player, we need an entity that emites projectiles
			m_player = std::make_unique<Entity>(m_registry->CreateEntity());
			m_player->Tag("player");
			m_player->AddComponent<TransformComponent>();
			m_player->AddComponent<ProjectileEmitterComponent>();
			// infinite fire rate
			m_player->GetComponent<ProjectileEmitterComponent>().m_frequencyInMs = 0;

			// setup event bus and callbacks needed to trigger the projectile's spawnage
			m_eventBus = std::make_unique<EventBus>();
			m_registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(m_eventBus);

			m_registry->Update();
		}

		std::unique_ptr<EventBus> m_eventBus;
		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<Entity> m_player;

		////////////////////////// DI?
		void PlayerProjectileFiringWithInfiniteFireRateSetup::FireProjectile(float timeButtonHeldDownInSecs = 0)
		{
			SDL_Rect camera{ 100,0,0,0 };
			m_eventBus->EmitEvent<LeftMouseButtonDownEvent>(camera);
			SDL_Delay(timeButtonHeldDownInSecs * 1000);
			m_eventBus->EmitEvent<LeftMouseButtonUpEvent>(camera);
			m_registry->Update();
		};

		float GetLastFiredProjectileVelocityMagnitude() const
		{
			const auto& activeProjectiles = m_registry->GetSystem<ProjectileLifeCycleSystem>().GetSystemEntities();
			return glm::length(activeProjectiles.at(activeProjectiles.size() - 1).GetComponent<RigidbodyComponent>().m_velocity);
		}

		int GetNumberOfCreatedProjectiles() const
		{
			return m_registry->GetSystem<ProjectileLifeCycleSystem>().GetSystemEntities().size(); 
		}

		float GetMinimumProjectileVelocityMagnitude() const
		{
			return m_player->GetComponent<ProjectileEmitterComponent>().GetMinProjectileVelocity();
		}

		void SetTimeToReachMaxVelocity(float timeToReachMaxVelocityInSecs)
		{
			auto& playerProjectileEmitterComponent = m_player->GetComponent<ProjectileEmitterComponent>();
			playerProjectileEmitterComponent.SetTimeToReachMaxVelocity(timeToReachMaxVelocityInSecs);
		}

		float GetMaximumProjectileVelocityMagnitude() const
		{
			auto& playerProjectileEmitterComponent = m_player->GetComponent<ProjectileEmitterComponent>();
			return playerProjectileEmitterComponent.GetMaxProjectileVelocity();
		}

		//////////////////////// DI ???
	};

	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, GivenNoProjectilesExist_When1ProjectileIsFired_ThenNumberOfInstantiatedProjectilesIs1)
	{
		FireProjectile();

		ASSERT_EQ(1, GetNumberOfCreatedProjectiles());
	}
	
	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, WhenMultiplesProjectilesAreFired_ThenTheNumberOfInstantiatedProjectilesIsEqualToThoseManyProjectiles)
	{
		const Uint16 numberOfProjectilesToFire = 10;
		for (int i = 0; i < numberOfProjectilesToFire; i++)
		{
			FireProjectile();
		}

		ASSERT_EQ(numberOfProjectilesToFire, GetNumberOfCreatedProjectiles());
	}

	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, WhenFireProjectileButtonTapped_ThenProjectileVelocityIsMinimum)
	{
		const float timeButtonHeldDownInSecs = .0f;
		FireProjectile(timeButtonHeldDownInSecs);

		ASSERT_FLOAT_EQ(GetLastFiredProjectileVelocityMagnitude(), GetMinimumProjectileVelocityMagnitude());
	}

	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, WhenFireProjectileButtonWasHeldForLongerThanTimeToReachMaxVelocity_ThenProjectileVelocityIsMax)
	{
		const float timeToReachMaxVelocityInSecs = .1f;
		SetTimeToReachMaxVelocity(timeToReachMaxVelocityInSecs);

		const float timeButtonHeldDownInSecs = .11f;
		FireProjectile(timeButtonHeldDownInSecs);

		ASSERT_FLOAT_EQ(GetLastFiredProjectileVelocityMagnitude(), GetMaximumProjectileVelocityMagnitude());
	}
}