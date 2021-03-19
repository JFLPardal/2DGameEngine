#include "Game.h"

#include <iostream>
#include <SDL.h>

Game::Game()
{
    std::cout << "game created" << std::endl;
}

void Game::Initialize()
{
    bool errorInitializingSDL = SDL_Init(SDL_INIT_EVERYTHING) != 0;
    if (errorInitializingSDL)
    {
        std::cerr << "Error initializing SDL" << std::endl;
        return;
    }

    SDL_Window* window = SDL_CreateWindow(
        "2D Game Engine", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_BORDERLESS
        );
    if(!window)
    {
        std::cerr << "Error creating SDL window" << std::endl;
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        std::cerr << "Error creating SDL renderer" << std::endl;
        return;
    }
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