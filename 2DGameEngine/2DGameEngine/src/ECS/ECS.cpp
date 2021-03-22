#include "ECS.h"

/// ----------------- ENTITY  ----------------------

Entity::Entity(int id)
	:m_id(id)
{
}

int Entity::GetId() const
{
	return m_id;
}

/// ----------------- COMPONENT ----------------------

unsigned int IComponent::m_nextId = 0;

template<typename T>
inline unsigned int Component<T>::GetId()
{
	const auto id = m_nextId++;
	return id;
}

/// ----------------- SYSTEM ----------------------
void System::AddEntity(Entity entityToAdd)
{

}

void System::RemoveEntity(Entity entityToRemove)
{

}

const std::vector<Entity>& System::GetSystemEntities() const
{
	return m_entities;
}

const Signature& System::GetComponentSignature() const
{
	return m_componentSignature;
}

/// ----------------- REGISTRY ----------------------