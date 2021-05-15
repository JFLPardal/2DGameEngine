#pragma once

#include "sol/sol.hpp"

#include "ECS/ECS.h"

#include "Components/ScriptComponent.h"
#include "Components/TransformComponent.h"

//// declaration of native c++ functions that we will bind with lua functions
//int GetEntityPosition(Entity entity)
//{
//
//}

void SetEntityPosition(Entity entity, double newX, double newY)
{
	if (entity.HasComponent<TransformComponent>())
	{
		auto& position = entity.GetComponent<TransformComponent>().m_position;
		position.x = newX;
		position.y = newY;
	}
	else
	{
		Logger::Error("trying to set the position of an entity that does not have a transform component");
	}
}

class ScriptSystem : public System
{
public:
	ScriptSystem()
	{
		RequireComponent<ScriptComponent>();
	}

	void Update(double deltaTime, int ellapsedTime)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto script = entity.GetComponent<ScriptComponent>();
			script.m_scriptFunction(entity, deltaTime, ellapsedTime);
		}
	}

	void CreateLuaFunctionBindings(sol::state& lua)
	{
		// create entity usertype so that Lua knows what an entity is
		lua.new_usertype<Entity>(
			"entity",
			"get_id", &Entity::GetId,
			"destroy", &Entity::Destroy,
			"has_tag", &Entity::HasTag,
			"belongs_to_group", &Entity::BelongsToGroup
		);

		lua.set_function("set_position", SetEntityPosition);
	}
};