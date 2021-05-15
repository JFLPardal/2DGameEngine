#pragma once

#include <sol/sol.hpp>

struct ScriptComponent
{
	sol::function m_scriptFunction;

	ScriptComponent(sol::function scriptFunction = sol::lua_nil)
		:m_scriptFunction(scriptFunction)
	{
	}
};