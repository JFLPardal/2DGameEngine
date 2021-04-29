#include "Game.h"

#include <fstream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>

#include "Logger/Logger.h"

#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/KeyboardControlledComponent.h"
#include "Components/CameraFollowComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/HealthComponent.h"
#include "Components/TextLabelComponent.h"

#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/RenderColliderSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/KeyboardControlSystem.h"
#include "Systems/CameraMovementSystem.h"
#include "Systems/ProjectileEmitSystem.h"
#include "Systems/ProjectileLifeCycleSystem.h"
#include "Systems/RenderTextSystem.h"
#include "Systems/RenderHealthBarSystem.h"

int Game::m_windowWidth = 1680;
int Game::m_windowHeight = 1050;
int Game::m_mapWidth;
int Game::m_mapHeight;

Game::Game()
    : m_IsRunning(false)
    , m_assetStore(std::make_unique<AssetStore>())
    , m_registry(std::make_unique<Registry>())
    , m_eventBus(std::make_unique<EventBus>())
    , m_camera(SDL_Rect{0,0, m_windowWidth, m_windowHeight})
{
    Logger::Log("game created");
}

void Game::Initialize()
{
    // init SDL
    const bool errorInitializingSDL = SDL_Init(SDL_INIT_EVERYTHING) != 0;
    if (errorInitializingSDL)
    {
        Logger::Error("Error initializing SDL" );
        return;
    }

    const bool errorInitializingSDL_TTF = TTF_Init() != 0;
    if (errorInitializingSDL_TTF)
    {
        Logger::Error("Error initializing SDL TTF");
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

    // initialize Imgui
    ImGui::CreateContext();
    ImGuiSDL::Initialize(m_renderer, m_windowWidth, m_windowHeight);

    // initialize the camera view with the entire screen area
    m_camera = SDL_Rect{0,0, m_windowWidth, m_windowHeight};

    //SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    m_IsRunning = true;
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        // imgui sdl input
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();
        {
            // imgui mouse needs
            int mouseX, mouseY;
            const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
            io.MousePos = ImVec2(mouseX, mouseY);
            io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
            io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        }

        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            m_IsRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                m_IsRunning = false;
            }
            else if (sdlEvent.key.keysym.sym == SDL_KeyCode::SDLK_c)
            {
                m_shouldRenderDebug = !m_shouldRenderDebug;
            }
            m_eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
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
    m_registry->AddSystem<KeyboardControlSystem>();
    m_registry->AddSystem<CameraMovementSystem>();
    m_registry->AddSystem<ProjectileEmitSystem>();
    m_registry->AddSystem<ProjectileLifeCycleSystem>();
    m_registry->AddSystem<RenderTextSystem>();
    m_registry->AddSystem<RenderHealthBarSystem>();

    // add assets to assetStore
    m_assetStore->AddTexture(m_renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    m_assetStore->AddTexture(m_renderer, "radar-image", "./assets/images/radar.png");
    m_assetStore->AddTexture(m_renderer, "tank-image", "./assets/images/tank-panther-right.png");
    m_assetStore->AddTexture(m_renderer, "truck-image", "./assets/images/truck-ford-right.png");
    m_assetStore->AddTexture(m_renderer, "jungle-tileset", "./assets/tilemaps/jungle.png");
    m_assetStore->AddTexture(m_renderer, "bullet-image", "./assets/images/bullet.png");
    m_assetStore->AddFont(CONST::FONT::charriot_20, "./assets/fonts/charriot.ttf", 20);
    m_assetStore->AddFont(CONST::FONT::pico_8, "./assets/fonts/pico8.ttf", 8);
    m_assetStore->AddFont(CONST::FONT::pico_10, "./assets/fonts/pico8.ttf", 10);

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
            tile.Tag("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0);
            tile.AddComponent<SpriteComponent>("jungle-tileset", tileSize, tileSize, 0, false, tilemapXIndex, tilemapYIndex);
        }
    }
    mapFile.close();

    m_mapWidth = tileSize * tileScale * mapNumCols;
    m_mapHeight = tileSize * tileScale * mapNumRows;

    // create entities
    Entity chopper = m_registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(20, 400), glm::vec2(1, 1), 0);
    chopper.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 12, true);
    const auto movementSpeed = 100;
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0,-movementSpeed), glm::vec2(movementSpeed,0), glm::vec2(0, movementSpeed), glm::vec2(-movementSpeed, 0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(500, 500), 0, 10000, 10);
    chopper.AddComponent<TextLabelComponent>(glm::vec2(0,-25), "100%", CONST::FONT::pico_10, CONST::COLORS::lightGrey, false);

    Entity radar = m_registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(m_windowWidth - 74, 10), glm::vec2(1, 1), 0);
    radar.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 3, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);
    
    Entity tank = m_registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(300, 20), glm::vec2(1, 1), 0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100, 0), 300, 6000, 25, true);
    tank.AddComponent<HealthComponent>();
    tank.AddComponent<TextLabelComponent>(glm::vec2(0, -25), "100%", CONST::FONT::pico_10, CONST::COLORS::lightGrey, false);

    Entity truck = m_registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(200, 200), glm::vec2(1, 1), 0);
    truck.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0, 50), 3000, 15000, 25, true);
    truck.AddComponent<HealthComponent>();
    truck.AddComponent<TextLabelComponent>(glm::vec2(0, -25), "100%", CONST::FONT::pico_10, CONST::COLORS::lightGrey, false);

    Entity textLabel = m_registry->CreateEntity();
    SDL_Color lightGrey = { 200, 200, 200, 255 };
    textLabel.AddComponent<TextLabelComponent>(glm::vec2(m_windowWidth * .5f - 100 ,20), "GAME ENGINE SHOWCASE", CONST::FONT::charriot_20, lightGrey, true);

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
    m_registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(m_eventBus);
    m_registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(m_eventBus);

    // update the registry to add or remove entities that were waiting for the end of the frame
    m_registry->Update();

    m_registry->GetSystem<MovementSystem>().Update(deltaTime);
    m_registry->GetSystem<AnimationSystem>().Update();
    m_registry->GetSystem<CollisionSystem>().Update(m_eventBus);
    m_registry->GetSystem<ProjectileEmitSystem>().Update(m_registry);
    m_registry->GetSystem<ProjectileLifeCycleSystem>().Update();
    m_registry->GetSystem<CameraMovementSystem>().Update(m_camera);
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

    m_registry->GetSystem<RenderSystem>().Update(m_renderer, m_assetStore, m_camera);
    m_registry->GetSystem<RenderTextSystem>().Update(m_assetStore, m_renderer, m_camera);
    m_registry->GetSystem<RenderHealthBarSystem>().Update(m_renderer, m_assetStore, m_camera);
    if (m_shouldRenderDebug)
    {
        m_registry->GetSystem<RenderColliderSystem>().Update(m_renderer, m_camera);
        
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }

    SDL_RenderPresent(m_renderer);
}

void Game::Destroy()
{
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}