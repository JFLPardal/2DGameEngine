#pragma once
#include "glm/glm.hpp"

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
class Registry;

class Game
{
public:
	Game();
	~Game() = default;

	void Initialize();
	void Run();
	void Destroy();

	int m_windowWidth = 800;
	int m_windowHeight = 600;
private:
	void ProcessInput();
	void Update();
	void Render();

	void Setup();

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	Registry* m_registry = nullptr;

	bool m_IsRunning = false; 
	int m_millisecondsPreviousFrame = 0;
};

