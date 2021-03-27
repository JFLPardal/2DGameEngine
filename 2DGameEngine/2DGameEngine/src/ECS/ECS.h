#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

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
	Entity(int id);

	bool operator==(const Entity& other) const;

	int GetId() const;

private:
	int m_id;
};

struct IComponent
{
protected:
	static unsigned int m_nextId;
};

// used to assign a unique id to a component type
template <typename T>
class Component : public IComponent
{
public:
	static unsigned int GetId();
};

class System
{
public:
	System() = default;
	~System() = default;

	void AddEntity(Entity entityToAdd);
	void RemoveEntity(Entity entityToRemove);

	const std::vector<Entity>& GetSystemEntities() const;
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
	~IPool() = delete;
};

// wrapper around a vector
template <typename T>
class Pool : public IPool
{
public:
	Pool(unsigned int size = 100)
	{
		m_data.resize(size);
	}
	
	virtual ~Pool() = default;

	bool IsEmpty() const { return m_data.empty(); }
	bool GetSize() const { return m_data.size(); }
	void Resize(unsigned int size) { m_data.resize(size); }
	void Clear() { m_data.clear(); }

	void Add(T objectToAdd) { m_data.push_back(objectToAdd); }
	void Set(unsigned int indexToSet, T objectToSet) { m_data.at(indexToSet) = objectToSet; }
	T& Get(unsigned int indexToGet) { return static_cast<T&>(m_data.at(indexToGet)); }
	T& operator[](unsigned int indexToGet) { return m_data.at(indexToGet); }
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

	// component management
	template <typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template<typename TComponent>
	void RemoveComponent(Entity entity);

	template <typename TComponent>
	bool HasComponent(Entity entity) const;

	// system management
	template <typename TSystem, typename ...TArgs> 
	void AddSystem(TArgs&& ...args);

	template <typename TSystem> 
	void RemoveSystem();

	template <typename TSystem> 
	bool HasSystem() const;

	template <typename TSystem> 
	TSystem& GetSystem() const;

	void AddEntityToSystem(Entity entityToAdd);

private:
	void Update();

	unsigned int m_numEntities = 0;

	// stores the component's signature for each entity.
	// this informs us of which components each entity has
	// [ vector index = entity id ]
	std::vector<Signature> m_entityComponentSignatures;

	// these sets serve as temp Entity holder so that  they are removed/added at the end 
	// of the frame rather than at any random time during the frame
	std::set<Entity> m_entitiesToAdd;
	std::set<Entity> m_entitiesToDestroy;

	// each pool contains all the information for a certain component type
	// [ vector index = component type id ]
	// [ pool index = entity id ]
	std::vector<IPool*> m_componentPools;

	std::unordered_map<std::type_index, System*> m_systems;
};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	// check if the component already exists in the componentPool
	// resize the componentPool if it does not
	const bool componentAlreadyExists = m_componentPools.size() >= componentId;
	if (!componentAlreadyExists)
	{
		m_componentPools.resize(m_componentPools.size() + 1, nullptr);
	}

	const Pool<TComponent>* componentToAddPool = m_componentPools.at(componentId);
	if (componentToAddPool == nullptr)
	{
		componentToAddPool = new Pool<TComponent>();
	}

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
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	
	m_entityComponentSignatures.at(entityId).set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	
	return m_entityComponentSignatures.at(entityId).test(componentId);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
	TSystem* newSystem(new System(std::forward<TArgs>(args)...));

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
	const auto systemToGet = m_systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(systemToGet->second));
}