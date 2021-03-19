#pragma once

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
private:
	void ProcessInput();
	void Update();
	void Render();

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	bool m_IsRunning = false;
};

