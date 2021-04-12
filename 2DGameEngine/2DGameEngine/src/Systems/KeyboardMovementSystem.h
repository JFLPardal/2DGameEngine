#pragma once

#include "ECS/ECS.h"
#include "EventBus/EventBus.h"

#include "Events/KeyPressedEvent.h"

class KeyboardMovementSystem : public System
{
public:
	KeyboardMovementSystem() = default;

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& keyPressedEvent)
	{
		Logger::Log("pressed key: " + std::to_string(keyPressedEvent.m_keyPressed));
	}
};