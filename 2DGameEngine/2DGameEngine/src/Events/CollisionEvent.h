#pragma once

#include "ECS/ECS.h"
#include "EventBus/Event.h"

class CollisionEvent : public Event
{
public:
	Entity m_a;
	Entity m_b;

	CollisionEvent(Entity a, Entity b) 
		: m_a(a)
		, m_b(b)
	{}
};