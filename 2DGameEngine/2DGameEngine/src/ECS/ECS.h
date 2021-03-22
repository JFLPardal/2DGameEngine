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

	int GetId() const;

private:
	int m_id;
};

struct IComponent
{
protected:
	static unsigned int m_nextId;
};

// used o assign a unique id to a component type
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

class Registry
{

};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_componentSignature.set(componentId);
}