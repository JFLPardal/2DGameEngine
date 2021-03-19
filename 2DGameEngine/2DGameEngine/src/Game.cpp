#include "Game.h"
#include <iostream>

Game::Game()
{
    std::cout << "game created" << std::endl;
}

void Game::Initialize()
{
}

void Game::Update()
{
}

void Game::Run()
{
    while (true)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Render()
{
}

void Game::Destroy()
{
}

void Game::ProcessInput()
{
}