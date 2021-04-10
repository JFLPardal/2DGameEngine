#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "ECS/ECS.h"
#include "AssetStore/AssetStore.h"
#include "EventBus/EventBus.h"

namespace CONST
{
	namespace FRAMERATE
	{
		constexpr bool IS_CAPPED = true;
		constexpr int FPS = 120;
		constexpr int MILLISECS_PER_FRAME = 1000 / CONST::FRAMERATE::FPS;
	}
}

struct SDL_Window;
struct SDL_Renderer;

class Game
{
public:
	Game();
	~Game() = default;

	void Initialize();
	void Run();
	void Destroy();

	int m_windowWidth = 1200;
	int m_windowHeight = 800;
private:
	void ProcessInput();
	void Update();
	void Render();

	void Setup();
	void LoadLevel(Uint8 levelNumber);

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	std::unique_ptr<Registry> m_registry;
	std::unique_ptr<AssetStore> m_assetStore;
	std::unique_ptr<EventBus> m_eventBus;

	bool m_IsRunning = false; 
	int m_millisecondsPreviousFrame = 0;

	bool m_shouldRenderDebug = false;
};

