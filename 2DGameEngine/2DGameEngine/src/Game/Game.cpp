#include "Game.h"

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "Logger/Logger.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"

Game::Game()
    : m_IsRunning(false)
    , m_assetStore(std::make_unique<AssetStore>())
    , m_registry(std::make_unique<Registry>())
{
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
    // add system to the game
    m_registry->AddSystem<MovementSystem>();
    m_registry->AddSystem<RenderSystem>();

    m_assetStore->AddTexture(m_renderer, "tank-image", "./assets/images/tank-panther-right.png");
    m_assetStore->AddTexture(m_renderer, "truck-image", "./assets/images/truck-ford-right.png");

    Entity tank = m_registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(100, 200), glm::vec2(1, 1), 0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(20, 8));
    tank.AddComponent<SpriteComponent>("tank-image", 10, 10);

    Entity truck = m_registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(500, 400), glm::vec2(1, 1), 0);
    truck.AddComponent<RigidbodyComponent>(glm::vec2(-50, -20));
    truck.AddComponent<SpriteComponent>("truck-image", 30, 20);
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

    m_registry->GetSystem<MovementSystem>().Update(deltaTime);

    // update the registry to add or remove entities that were waiting for the end of the frame
    m_registry->Update();
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

    m_registry->GetSystem<RenderSystem>().Update(m_renderer);

    SDL_RenderPresent(m_renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}