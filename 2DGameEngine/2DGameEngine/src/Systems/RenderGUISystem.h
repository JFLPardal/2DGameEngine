#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

#include "ECS/ECS.h"

class RenderGUISystem : public System
{
public:
	// not interested in any specific components so no need for RequireComponent<>()
	RenderGUISystem() = default;

	void Update()
	{
		using namespace ImGui;

		NewFrame();

		if (Begin("Enemy Spawner"))
		{
			Button("spawn", ImVec2(50, 20));
		}
		End();

		Render();
		ImGuiSDL::Render(GetDrawData());
	}
};