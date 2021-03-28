#include "Game.h"

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "Logger/Logger.h"
#include "ECS/ECS.h"

Game::Game()
    :m_IsRunning(false)
{
    m_registry = new Registry();
    Logger::Log("game created");
}

void Game::Initialize()
{
    // init SDL
    bool errorInitializingSDL = SDL_Init(SDL_INIT_EVERYTHING) != 0;
    if (errorInitializingSDL)
    {
        Logger::Error("Error initializing SDL" );
        return;
    }

    // init window
    // setting the w and h here before using SDL_SetWindowFullscreen()
    // will set the resolution of the window
    m_window = SDL_CreateWindow(
        "2D Game Engine", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        m_windowWidth,
        m_windowHeight,
        SDL_WINDOW_BORDERLESS
        );
    if(!m_window)
    {
        Logger::Error("Error creating SDL window");
        return;
    }

    // init renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if (!m_renderer)
    {
        Logger::Error("Error creating SDL renderer");
        return;
    }

    //SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    m_IsRunning = true;
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            m_IsRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                m_IsRunning = false;
            }
            break;
        }
    }
}

void Game::Setup()
{
    Entity tank = m_registry->CreateEntity();
    Entity truck = m_registry->CreateEntity();
}

void Game::Update()
{
    // frame management
    if (CONST::FRAMERATE::IS_CAPPED)
    {
        int millisecondsToWait = CONST::FRAMERATE::MILLISECS_PER_FRAME - (SDL_GetTicks() - m_millisecondsPreviousFrame);
        if (millisecondsToWait > 0 && millisecondsToWait <= CONST::FRAMERATE::MILLISECS_PER_FRAME)
        {
            SDL_Delay(millisecondsToWait);
        }
    }

    double deltaTime = (SDL_GetTicks() - m_millisecondsPreviousFrame) / 1000.0;
    m_millisecondsPreviousFrame = SDL_GetTicks();

}

void Game::Run()
{
    Setup();
    while (m_IsRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
    SDL_RenderClear(m_renderer);

    SDL_RenderPresent(m_renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    delete m_registry;
}