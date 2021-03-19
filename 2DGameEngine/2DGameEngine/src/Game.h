#pragma once

// constants
namespace CONST
{
	constexpr int FPS = 60;
	constexpr int MILLISECS_PER_FRAME = 1000 / CONST::FPS;
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

	int m_windowWidth = 800;
	int m_windowHeight = 600;
private:
	void ProcessInput();
	void Update();
	void Render();

	void Setup();

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	bool m_IsRunning = false; 
	int m_millisecondsPreviousFrame = 0;
};

