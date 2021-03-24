#include "ECS.h"

/// ----------------- ENTITY  ----------------------

Entity::Entity(int id)
	:m_id(id)
{
}

bool Entity::operator==(const Entity& other) const
{
	return GetId() == other.GetId();
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
	m_entities.push_back(entityToAdd);
}

void System::RemoveEntity(Entity entityToRemove)
{
	m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(), 
			[&entityToRemove](Entity other) 
			{ 
				return other == entityToRemove; 
			}
			), 
		m_entities.end());
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