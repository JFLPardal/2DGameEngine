#include "Game.h"

#include <fstream>

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "Logger/Logger.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/BoxColliderComponent.h"

#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/RenderColliderSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/KeyboardMovementSystem.h"

Game::Game()
    : m_IsRunning(false)
    , m_assetStore(std::make_unique<AssetStore>())
    , m_registry(std::make_unique<Registry>())
    , m_eventBus(std::make_unique<EventBus>())
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
            else if (event.key.keysym.sym == SDL_KeyCode::SDLK_d)
            {
                m_shouldRenderDebug = !m_shouldRenderDebug;
            }
            m_eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);
            break;
        }
    }
}

void Game::Setup()
{
    LoadLevel(1);
}

void Game::LoadLevel(Uint8 levelNumber)
{
    // add system to the game
    m_registry->AddSystem<MovementSystem>();
    m_registry->AddSystem<RenderSystem>();
    m_registry->AddSystem<AnimationSystem>();
    m_registry->AddSystem<CollisionSystem>();
    m_registry->AddSystem<RenderColliderSystem>();
    m_registry->AddSystem<DamageSystem>();
    m_registry->AddSystem<KeyboardMovementSystem>();

    // add assets to assetStore
    m_assetStore->AddTexture(m_renderer, "chopper-image", "./assets/images/chopper.png");
    m_assetStore->AddTexture(m_renderer, "radar-image", "./assets/images/radar.png");
    m_assetStore->AddTexture(m_renderer, "tank-image", "./assets/images/tank-panther-right.png");
    m_assetStore->AddTexture(m_renderer, "truck-image", "./assets/images/truck-ford-right.png");
    m_assetStore->AddTexture(m_renderer, "jungle-tileset", "./assets/tilemaps/jungle.png");

    // load tilemap
    const Uint8 tileSize = 32;
    double tileScale = 2.0;
    const Uint8 mapNumCols = 25;
    const Uint8 mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");

    for (int y = 0; y < mapNumRows; y++)
    {
        for(int x = 0; x < mapNumCols; x++)
        {
            char ch;
            mapFile.get(ch);
            const int tilemapYIndex = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            const int tilemapXIndex = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = m_registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0);
            tile.AddComponent<SpriteComponent>("jungle-tileset", tileSize, tileSize, 0,  tilemapXIndex, tilemapYIndex);
        }
    }
    mapFile.close();

    // create entities
    Entity chopper = m_registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(20, 400), glm::vec2(1, 1), 0);
    chopper.AddComponent<RigidbodyComponent>(glm::vec2(30, 0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 12, true);
    
    Entity radar = m_registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(m_windowWidth - 74, 10), glm::vec2(1, 1), 0);
    radar.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 3);
    radar.AddComponent<AnimationComponent>(8, 5, true);
    
    Entity tank = m_registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(300, 20), glm::vec2(1, 1), 0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(0, 55));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32);

    Entity truck = m_registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(300, 200), glm::vec2(1, 1), 0);
    truck.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
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
    
    // subscription to events will only be alive during one frame
    // suboptimal, should def be changed
    m_eventBus->Reset();
    m_registry->GetSystem<DamageSystem>().SubscribeToEvents(m_eventBus);
    m_registry->GetSystem<KeyboardMovementSystem>().SubscribeToEvents(m_eventBus);

    // update the registry to add or remove entities that were waiting for the end of the frame
    m_registry->Update();

    m_registry->GetSystem<MovementSystem>().Update(deltaTime);
    m_registry->GetSystem<AnimationSystem>().Update();
    m_registry->GetSystem<CollisionSystem>().Update(m_eventBus);

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

    m_registry->GetSystem<RenderSystem>().Update(m_renderer, m_assetStore);
    if (m_shouldRenderDebug)
    {
        m_registry->GetSystem<RenderColliderSystem>().Update(m_renderer);
    }

    SDL_RenderPresent(m_renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}