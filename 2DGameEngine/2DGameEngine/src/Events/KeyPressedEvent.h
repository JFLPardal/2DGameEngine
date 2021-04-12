#pragma once

#include "SDL.h"

#include "EventBus/Event.h"

class KeyPressedEvent : public Event
{
public:
	SDL_Keycode m_keyPressed;

	KeyPressedEvent(SDL_Keycode keyPressed)
		: m_keyPressed(keyPressed)
	{
	}
};