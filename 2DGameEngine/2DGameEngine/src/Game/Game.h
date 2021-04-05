#pragma once
#include "glm/glm.hpp"
#include <memory>
#include "ECS/ECS.h"
#include "AssetStore/AssetStore.h"

namespace CONST
{
	namespace FRAMERATE
	{
		constexpr bool IS_CAPPED = true;
		constexpr int FPS = 120;
		constexpr int MILLISECS_PER_FRAME = 1000 / CONST::FRAMERATE::FPS;
	}
}

class SDL_Window;
class SDL_Renderer;

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

	bool m_IsRunning = false; 
	int m_millisecondsPreviousFrame = 0;
};

