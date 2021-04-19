#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>

#include "Logger/Logger.h"

namespace CONST
{
	namespace ECS
	{
		constexpr unsigned int MAX_COMPONENTS = 32;
	}
}

// Signature represents the components that a Entity has or that
// an entity must have for a system to work on it
typedef std::bitset<CONST::ECS::MAX_COMPONENTS> Signature;

class Entity
{
public:
	Entity(std::size_t id);
	void Destroy();

	bool operator==(const Entity& other) const;
	bool operator!=(const Entity& other) const;
	bool operator< (const Entity& other) const;

	std::size_t GetId() const;

	template <typename TComponent, typename ...TArgs>
	void AddComponent(TArgs&&... args);
	template <typename TComponent>
	void RemoveComponent();
	template <typename TComponent>
	bool HasComponent() const;
	template <typename TComponent>
	TComponent& GetComponent() const;

	class Registry* m_registry = nullptr; // forward declaration
private:
	std::size_t m_id;
};

struct IComponent
{
protected:
	static std::size_t m_nextId;
};

// used to assign a unique id to a component type
// ALL COMPONENTS CTOR NEED DEFAULT VALUES OR AN ERROR WILL OCCUR
template <typename T>
class Component : public IComponent
{
public:
	static std::size_t GetId();
};

template<typename T>
inline std::size_t Component<T>::GetId()
{
	static auto id = m_nextId++;
	return id;
}

class System
{
public:
	System() = default;
	~System() = default;

	void AddEntity(Entity entityToAdd);
	void RemoveEntity(Entity entityToRemove);

	std::vector<Entity>& GetSystemEntities();
	const Signature& GetComponentSignature() const;

	// what components will be a part of this system's signature?
	template <typename TComponent> void RequireComponent();
private:
	Signature m_componentSignature;
	std::vector<Entity> m_entities;
};

// this class is used so that in 'Registry' we are not forced to specify
// the Pool data type
class IPool
{
public:
	virtual ~IPool() {};
};

// wrapper around a vector
template <typename T>
class Pool : public IPool
{
public:
	Pool(std::size_t size = 100)
	{
		m_data.resize(size);
	}
	
	virtual ~Pool() = default;

	bool IsEmpty() const { return m_data.empty(); }
	std::size_t GetSize() const { return m_data.size(); }
	void Resize(std::size_t size) { m_data.resize(size); }
	void Clear() { m_data.clear(); }

	void Add(T objectToAdd) { m_data.push_back(objectToAdd); }
	void Set(std::size_t indexToSet, T objectToSet) { m_data.at(indexToSet) = objectToSet; }
	T& Get(std::size_t indexToGet) { return static_cast<T&>(m_data.at(indexToGet)); }
	T& operator[](std::size_t indexToGet) { return m_data.at(indexToGet); }
private:
	std::vector<T> m_data;
};

// manages creation and destruction of entities, add systems,
// and components
class Registry
{
public:
	Registry() = default;

	// entity management
	Entity CreateEntity();
	void DestroyEntity(Entity& entityToDestroy);

	// component management
	template <typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template<typename TComponent>
	void RemoveComponent(Entity entity);

	template <typename TComponent>
	bool HasComponent(Entity entity) const;
	
	template <typename TComponent>
	TComponent& GetComponent(Entity entity) const;

	// system management
	template <typename TSystem, typename ...TArgs>
	void AddSystem(TArgs&& ...args);

	template <typename TSystem> 
	void RemoveSystem();

	template <typename TSystem> 
	bool HasSystem() const;

	template <typename TSystem> 
	TSystem& GetSystem() const;

	void AddEntityToSystems(Entity entityToAdd);
	void RemoveEntityFromSystems(Entity entityToAdd);

	void Update();

private:
	std::size_t m_numEntities = 0;

	// stores the component's signature for each entity.
	// this informs us of which components each entity has
	// [ vector index = entity id ]
	std::vector<Signature> m_entityComponentSignatures;

	// these sets serve as temp Entity holder so that  they are removed/added at the end 
	// of the frame rather than at any random time during the frame
	std::set<Entity> m_entitiesToAdd;
	std::set<Entity> m_entitiesToDestroy;

	// list of free ids from previously destroyed entitites
	std::deque<std::size_t> m_freeIds;

	// each pool contains all the information for a certain component type
	// [ vector index = component type id ]
	// [ pool index = entity id ]
	std::vector<std::shared_ptr<IPool>> m_componentPools;

	std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;
};


// ------------------------------------ ENTITY TEMPLATE FUNCTIONS-----------------

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&&... args)
{
	m_registry->AddComponent<TComponent>(*this, args...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
	m_registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return m_registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return m_registry->GetComponent<TComponent>(*this);
}


// ------------------------------------ SYSTEM TEMPLATE FUNCTIONS-----------------

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_componentSignature.set(componentId);
}


// ------------------------------------ REGISTRY TEMPLATE FUNCTIONS-----------------

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	// check if the component already exists in the componentPool
	// resize the componentPool if it does not
	const bool componentAlreadyExists = m_componentPools.size() > componentId;
	if (!componentAlreadyExists)
	{
		m_componentPools.resize(componentId + 1, nullptr);
	}

	if (!m_componentPools.at(componentId))
	{
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		m_componentPools.at(componentId) = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentToAddPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools.at(componentId));
	// check if the pool is sized large enough to fit the new component
	const bool needsToAddComponentToPool = m_numEntities >= componentToAddPool->GetSize();
	if (needsToAddComponentToPool)
	{
		componentToAddPool->Resize(m_numEntities);
	}

	TComponent componentToAdd(std::forward<TArgs>(args)...);
	componentToAddPool->Set(entityId, componentToAdd);

	// update the entity's signature to have the added component
	m_entityComponentSignatures.at(entityId).set(componentId);

	//Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id = " + std::to_string(entityId));
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	
	m_entityComponentSignatures.at(entityId).set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id = " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	
	return m_entityComponentSignatures.at(entityId).test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools.at(componentId));

	return componentPool->Get(entityId);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
	std::shared_ptr<TSystem> newSystem =std::make_shared<TSystem>(std::forward<TArgs>(args)...);

	m_systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
	const auto sytemToRemoveIndex = m_systems.find(std::type_index(typeid(TSystem)));
	m_systems.erase(sytemToRemoveIndex);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
	const auto requestedSystemIndex = m_systems.find(std::type_index(typeid(TSystem)));
	return requestedSystemIndex != m_systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto systemToGet = m_systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(systemToGet->second));
}