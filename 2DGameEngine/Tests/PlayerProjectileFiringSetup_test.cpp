#include "pch.h"

#include <glm/glm.hpp>
#include <memory>
#include <chrono>
#include <thread>

#include "ECS/ECS.h"
#include "Game/Game.h"
#include "EventBus/EventBus.h"
#include "Events/LeftMouseButtonPressedEvent.h"

#include "Systems/MovementSystem.h"
#include "Systems/ProjectileLifeCycleSystem.h"
#include "Systems/ProjectileEmitSystem.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/ProjectileEmitterComponent.h"

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

		void PlayerProjectileFiringWithInfiniteFireRateSetup::FireProjectile()
		{
			SDL_Rect camera{ 100,0,0,0 };
			m_eventBus->EmitEvent<LeftMouseButtonDownEvent>(camera);
			m_eventBus->EmitEvent<LeftMouseButtonUpEvent>(camera);
			m_registry->Update();
		};

		std::unique_ptr<EventBus> m_eventBus;
		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<Entity> m_player;

		const Entity& GetLastFiredProjectile() const
		{
			const auto& activeProjectiles = m_registry->GetSystem<ProjectileLifeCycleSystem>().GetSystemEntities();
			return activeProjectiles.at(activeProjectiles.size() - 1);
		}
	};

	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, When1ProjectileWasCreated_ThenProjectileLifeCycleSystemHas1Entity)
	{
		FireProjectile();

		ASSERT_EQ(1, m_registry->GetSystem<ProjectileLifeCycleSystem>().GetSystemEntities().size());
	}
	
	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, WhenMultiplesProjectilesWereFired_ThenProjectileLifeCycleSystemHasThoseManyProjectiles)
	{
		const Uint8 numberOfProjectilesToFire = 10;
		for (int i = 0; i < numberOfProjectilesToFire; i++)
		{
			FireProjectile();
		}

		ASSERT_EQ(numberOfProjectilesToFire, m_registry->GetSystem<ProjectileLifeCycleSystem>().GetSystemEntities().size());
	}

	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, WhenFireProjectileButtonTapped_ThenProjectileVelocityIsMinimum)
	{
		/*SDL_Rect camera{ 100,0,0,0 };
		m_eventBus->EmitEvent<LeftMouseButtonDownEvent>(camera);
		m_eventBus->EmitEvent<LeftMouseButtonUpEvent>(camera);*/
		FireProjectile();

		float minimumProjectileVelocityMagnitude = m_player->GetComponent<ProjectileEmitterComponent>().GetMinProjectileVelocity();
		float projectileVelocityMagnitude = glm::length(GetLastFiredProjectile().GetComponent<RigidbodyComponent>().m_velocity);
		
		ASSERT_FLOAT_EQ(minimumProjectileVelocityMagnitude, projectileVelocityMagnitude);
	}

	TEST_F(PlayerProjectileFiringWithInfiniteFireRateSetup, WhenFireProjectileButtonWasHeldForLongerThanTimeToReachMaxVelocity_ThenProjectileVelocityIsMax)
	{
		const float timeToReachMaxVelocityInSecs = .1f;

		auto& playerProjectileEmitterComponent = m_player->GetComponent<ProjectileEmitterComponent>();
		playerProjectileEmitterComponent.SetTimeToReachMaxVelocity(timeToReachMaxVelocityInSecs);

		SDL_Rect camera{ 100,0,0,0 };
		m_eventBus->EmitEvent<LeftMouseButtonDownEvent>(camera);
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(120ms);
		m_eventBus->EmitEvent<LeftMouseButtonUpEvent>(camera);
		m_registry->Update();

		float maximumProjectileVelocityMagnitude = playerProjectileEmitterComponent.GetMaxProjectileVelocity();
		float projectileVelocityMagnitude = glm::length(GetLastFiredProjectile().GetComponent<RigidbodyComponent>().m_velocity);

		ASSERT_FLOAT_EQ(maximumProjectileVelocityMagnitude, projectileVelocityMagnitude);
	}
}