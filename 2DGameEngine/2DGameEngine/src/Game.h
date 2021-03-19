#pragma once

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
};

