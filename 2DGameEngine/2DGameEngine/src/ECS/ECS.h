#pragma once

#include <bitset>
#include <vector>

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
private:
	// each pool contains all the information for a certain component type
	// vector index = component type id
	// pool index = entity id
	std::vector<IPool*> m_componentPools;
	unsigned int m_numEntities = 0;
};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_componentSignature.set(componentId);
}