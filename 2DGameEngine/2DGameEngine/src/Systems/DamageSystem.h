#pragma once

#include "ECS/ECS.h"

#include "EventBus/EventBus.h"
#include "Events/CollisionEvent.h"

#include "Components/BoxColliderComponent.h"

class DamageSystem : public System
{
public:
	DamageSystem()
	{
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);

	}

	void OnCollision(CollisionEvent& eventParams)
	{
		Logger::Log("CollisionEvent between: " + std::to_string(eventParams.m_a.GetId()) + " and " + std::to_string(eventParams.m_b.GetId()));
		eventParams.m_a.Destroy();
		eventParams.m_b.Destroy();
	}

	void Update()
	{

	}
};