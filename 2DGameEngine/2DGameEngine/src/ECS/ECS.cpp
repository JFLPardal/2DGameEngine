#include "ECS.h"

#include "Logger/Logger.h"

unsigned int IComponent::m_nextId = 0;

/// ----------------- ENTITY  ----------------------

Entity::Entity(int id)
	:m_id(id)
{
}

bool Entity::operator==(const Entity& other) const
{
	return GetId() == other.GetId();
}

bool Entity::operator<(const Entity& other) const
{
	return m_id < other.m_id;
}

int Entity::GetId() const
{
	return m_id;
}

/// ----------------- COMPONENT ----------------------

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

Entity Registry::CreateEntity()
{
	int entityId = m_numEntities++;
	Entity createdEntity(entityId);

	m_entitiesToAdd.insert(createdEntity);

	bool entityComponentSignatureNotBigEnough = entityId >= m_entityComponentSignatures.size();
	if (entityComponentSignatureNotBigEnough)
	{
		m_entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Log("Entity created with id: " + std::to_string(entityId));

	return createdEntity;
}

void Registry::AddEntityToSystem(Entity entityToAdd)
{
	const auto entityToAddId = entityToAdd.GetId();
	const auto& entityComponentSignature = m_entityComponentSignatures.at(entityToAddId);
	for (auto& system : m_systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();
		const bool entityHasSystemRequirements = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
		if (entityHasSystemRequirements)
		{
			system.second->AddEntity(entityToAdd);
		}
	}
}

void Registry::Update()
{
	for (auto entity : m_entitiesToAdd)
	{
		AddEntityToSystem(entity);
	}
	m_entitiesToAdd.clear();
}
