#pragma once

#include <map>
#include <list>
#include <typeindex>
#include <memory>

#include "Logger/Logger.h"
#include "Event.h"

// needs to exist because we can't have a unique_ptr of template types
class IEventCallback
{
public:
	virtual ~IEventCallback() = default;

	void Execute(Event& eventToExecute)
	{
		Call(eventToExecute);
	}

protected:
	virtual void Call(Event& eventToCall) = 0;
};

template<typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
	typedef void (TOwner::*CallbackFunction)(TEvent&);

public:
	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
		: m_ownerInstance(ownerInstance)
		, m_callbackFunction(callbackFunction)
	{
	}

	virtual ~EventCallback() override = default;

protected:
	virtual void Call(Event& eventToCall) override
	{
		std::invoke(m_callbackFunction, m_ownerInstance,  static_cast<TEvent>(eventToCall));
	}

private:
	TOwner* m_ownerInstance;
	CallbackFunction m_callbackFunction;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus
{
public:
	EventBus()
	{
		Logger::Log("EventBus constructor called");
	}

	~EventBus()
	{
		Logger::Log("EventBus destructor called");
	}

	// should be used in this format
	// eventBus->SubscribeToEvent<CollisionEvent>(this, &Game::OnCollision);
	template <typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner* ownerInstance, void(TOwner::*callbackFunction)(TEvent&))
	{
		if (m_subscribers.at(typeid(TEvent)).get() == nullptr)
		{
			m_subscribers.at(typeid(TEvent)) = std::make_unique<HandlerList>());
		}

		auto subscriberToAdd = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
		auto& listOfCallbacksForEventType = m_subscribers.at(typeid(TEvent));
		
		listOfCallbacksForEventType->push_back(std::move(subscriberToAdd));
	}

	template <typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args)
	{
		auto eventHandlers = m_subscribers.at(typeid(TEvent)).get();
		if (eventHandlers != nullptr)
		{
			for (auto it = eventHandlers->begin(); eventHandlers->end(); it++)
			{
				auto eventHandler = it->get();
				TEvent eventParams(std::forwards<TArgs>(args)...);
				eventHandler->Execute(eventParams);
			}
		}
	}
private:
	std::map<std::type_index, std::unique_ptr<HandlerList>> m_subscribers;
};