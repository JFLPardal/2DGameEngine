#include "LevelLoader.h"

#include <fstream>

#include <SDL.h>
#include <sol/sol.hpp>

#include "Game/Game.h"
#include "ECS/ECS.h"
#include "AssetStore/AssetStore.h"

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

namespace CONST
{
    namespace COLORS
    {
        constexpr SDL_Color lightGrey{ 200, 200, 200, 255 };
        constexpr SDL_Color orange{ 255, 165, 0, 255 };
        constexpr SDL_Color red{ 180, 0, 0, 255 };
    }
}

LevelLoader::LevelLoader()
{
}

void LevelLoader::LoadLevel(unsigned int levelToLoad, const std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer)
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // add assets to assetStore
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "jungle-tileset", "./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
    assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
    assetStore->AddFont(CONST::FONT::charriot_20, "./assets/fonts/charriot.ttf", 20);
    assetStore->AddFont(CONST::FONT::pico_8, "./assets/fonts/pico8.ttf", 8);
    assetStore->AddFont(CONST::FONT::pico_10, "./assets/fonts/pico8.ttf", 10);

    // load tilemap
    const Uint8 tileSize = 32;
    double tileScale = 2.0;
    const Uint8 mapNumCols = 25;
    const Uint8 mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");

    for (int y = 0; y < mapNumRows; y++)
    {
        for (int x = 0; x < mapNumCols; x++)
        {
            char ch;
            mapFile.get(ch);
            const int tilemapYIndex = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            const int tilemapXIndex = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.Tag("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0);
            tile.AddComponent<SpriteComponent>("jungle-tileset", tileSize, tileSize, 0, false, tilemapXIndex, tilemapYIndex);
        }
    }
    mapFile.close();

    Game::m_mapWidth = tileSize * tileScale * mapNumCols;
    Game::m_mapHeight = tileSize * tileScale * mapNumRows;

    // create entities
    Entity chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(20, 400), glm::vec2(1, 1), 0);
    chopper.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 12, true);
    const auto movementSpeed = 300;
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0, -movementSpeed), glm::vec2(movementSpeed, 0), glm::vec2(0, movementSpeed), glm::vec2(-movementSpeed, 0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(500, 500), 0, 10000, 10);
    chopper.AddComponent<TextLabelComponent>(glm::vec2(0, -25), "100%", CONST::FONT::pico_10, CONST::COLORS::lightGrey, false);

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(Game::m_windowWidth - 74, 10), glm::vec2(1, 1), 0);
    radar.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 3, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(300, 20), glm::vec2(1, 1), 0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(0, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100, 0), 300, 6000, 25, true);
    tank.AddComponent<HealthComponent>();
    tank.AddComponent<TextLabelComponent>(glm::vec2(0, -25), "100%", CONST::FONT::pico_10, CONST::COLORS::lightGrey, false);

    Entity truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(200, 500), glm::vec2(1, 1), 0);
    truck.AddComponent<RigidbodyComponent>(glm::vec2(30, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0, 50), 3000, 15000, 25, true);
    truck.AddComponent<HealthComponent>();
    truck.AddComponent<TextLabelComponent>(glm::vec2(0, -25), "100%", CONST::FONT::pico_10, CONST::COLORS::lightGrey, false);

    Entity tree01 = registry->CreateEntity();
    tree01.Group("obstacles");
    tree01.AddComponent<TransformComponent>(glm::vec2(300, 485));
    tree01.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    tree01.AddComponent<BoxColliderComponent>(16, 32);

    Entity tree02 = registry->CreateEntity();
    tree02.Group("obstacles");
    tree02.AddComponent<TransformComponent>(glm::vec2(150, 485));
    tree02.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    tree02.AddComponent<BoxColliderComponent>(16, 32);

    Entity textLabel = registry->CreateEntity();
    SDL_Color lightGrey = { 200, 200, 200, 255 };
    textLabel.AddComponent<TextLabelComponent>(glm::vec2(Game::m_windowWidth * .5f - 100, 20), "GAME ENGINE SHOWCASE", CONST::FONT::charriot_20, lightGrey, true);

}
