#include "ECS.h"

#include "Logger/Logger.h"

std::size_t IComponent::m_nextId = 0;

/// ----------------- ENTITY  ----------------------

Entity::Entity(std::size_t id)
	:m_id(id)
{
}

void Entity::Destroy()
{
	m_registry->DestroyEntity(*this);
}

bool Entity::operator==(const Entity& other) const
{
	return GetId() == other.GetId();
}

bool Entity::operator!=(const Entity& other) const
{
	return GetId() != other.GetId();
}

bool Entity::operator<(const Entity& other) const
{
	return GetId() < other.GetId();
}

std::size_t Entity::GetId() const
{
	return m_id;
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
	std::size_t entityId;

	const bool thereAreFreeIds = !m_freeIds.empty();
	if (thereAreFreeIds)
	{
		entityId = m_freeIds.front();
		m_freeIds.pop_front();
	}
	else
	{
		entityId = m_numEntities++;

		bool entityComponentSignatureNotBigEnough = entityId >= m_entityComponentSignatures.size();
		if (entityComponentSignatureNotBigEnough)
		{
			m_entityComponentSignatures.resize(entityId + 1);
		}
	}

	Entity createdEntity(entityId);
	createdEntity.m_registry = this;

	m_entitiesToAdd.insert(createdEntity);
	 
	//Logger::Log("Entity created with id: " + std::to_string(entityId));

	return createdEntity;
}

void Registry::DestroyEntity(Entity& entityToDestroy)
{
	m_entitiesToDestroy.insert(entityToDestroy);
}

void Registry::AddEntityToSystems(Entity entityToAdd)
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

void Registry::RemoveEntityFromSystems(Entity entityToRemove)
{
	for (auto& system : m_systems)
	{
		system.second->RemoveEntity(entityToRemove);
	}
}

void Registry::Update()
{
	// add new entities
	for (auto& entity : m_entitiesToAdd)
	{
		AddEntityToSystems(entity);
	}
	m_entitiesToAdd.clear();

	// destroy entities
	for (auto& entity : m_entitiesToDestroy)
	{
		RemoveEntityFromSystems(entity);
		m_entityComponentSignatures.at(entity.GetId()).reset();
		m_freeIds.push_back(entity.GetId());
	}
	m_entitiesToDestroy.clear(); 
}
