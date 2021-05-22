#pragma once

#include "SDL.h"

#include "EventBus/Event.h"

class LeftMouseButtonPressedEvent : public Event
{
public:
	SDL_Rect m_cameraPositon;

	LeftMouseButtonPressedEvent(const SDL_Rect& cameraPosition)
		: m_cameraPositon(cameraPosition)
	{
	}
};