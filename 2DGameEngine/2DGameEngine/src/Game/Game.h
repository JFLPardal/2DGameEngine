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
	namespace FONT
	{
		constexpr auto charriot_20 = "charriot-font";
		constexpr auto pico_8 = "pico-font";
		constexpr auto pico_10 = "pico-font";
	}
	namespace COLORS
	{
		constexpr SDL_Color lightGrey{ 200, 200, 200, 255 };
		constexpr SDL_Color orange{ 255, 165, 0, 255 };
		constexpr SDL_Color red{ 180, 0, 0, 255 };
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

	static int m_windowWidth;
	static int m_windowHeight;
	static int m_mapWidth;
	static int m_mapHeight;
private:
	void ProcessInput();
	void Update();
	void Render();

	void Setup();
	void LoadLevel(Uint8 levelNumber);

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	SDL_Rect m_camera;

	std::unique_ptr<Registry> m_registry;
	std::unique_ptr<AssetStore> m_assetStore;
	std::unique_ptr<EventBus> m_eventBus;

	bool m_IsRunning = false; 
	int m_millisecondsPreviousFrame = 0;

	bool m_shouldRenderDebug = false;
};

