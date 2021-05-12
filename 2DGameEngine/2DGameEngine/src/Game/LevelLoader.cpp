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

void LevelLoader::LoadLevel(unsigned int levelToLoad, const std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, sol::state& lua)
{
    // this call will check the script's syntax before executing it potentially avoid abortion
    // of the program if something went wrong by simply spitting a message out
    sol::load_result script = lua.load_file("./assets/scripts/Level" + std::to_string(levelToLoad) + ".lua");
    if (!script.valid())
    {
        sol::error error = script;
        std::string errorMessage = error.what();
        Logger::Error("Error loading lua script: "+ errorMessage);
        return;
    }

    // executes script
    lua.script_file("./assets/scripts/Level" + std::to_string(levelToLoad) + ".lua");

    sol::table level = lua["Level"];
    
    // read assets
    {
        sol::table assets = level["assets"];
        int i = 0;
        while (true)
        {
            //checks if assets[i] exists
            sol::optional<sol::table> hasAsset = assets[i];
            if (hasAsset == sol::nullopt)
            {
                break;
            }
            sol::table asset = assets[i];

            const std::string assetType = asset["type"];
            const std::string assetId = asset["id"];
            if (assetType == "texture")
            {
                assetStore->AddTexture(renderer, asset["id"], asset["file"]);
                Logger::Log("AssetStore: Added texture: " + assetId);
            }
            else if (assetType == "font")
            {
                const std::string fontSizeAsString = asset["font_size"];
                assetStore->AddFont(asset["id"], asset["file"], asset["font_size"]);
                Logger::Log("AssetStore: Added font: " + assetId + " with size: " + fontSizeAsString);
            }
            i++;
        }
    }
    
    // read map
    {
        const sol::table tilemap = level["tilemap"];
        // load tilemap
        const Uint8 tileSize = tilemap["tile_size"];
        double tileScale = tilemap["tile_scale"];
        const Uint8 mapNumCols = tilemap["num_cols"];
        const Uint8 mapNumRows = tilemap["num_rows"];

        const std::string mapPath = tilemap["map_file"];
        std::fstream mapFile;
        mapFile.open(mapPath);

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
                tile.AddComponent<SpriteComponent>(tilemap["texture_asset_id"], tileSize, tileSize, 0, false, tilemapXIndex, tilemapYIndex);
            }
        }
        mapFile.close();

        Game::m_mapWidth = tileSize * tileScale * mapNumCols;
        Game::m_mapHeight = tileSize * tileScale * mapNumRows;
    }

    // read entities
    {
        const sol::table entities = level["entities"];
        Uint8 i = 0;
        while (true)
        {
            // entity
            const sol::optional<sol::table> hasEntity = entities[i];
            if (hasEntity == sol::nullopt) break;
            auto newEntity = registry->CreateEntity();
            const sol::table entity = entities[i];

            // tag
            const sol::optional<std::string> hasTag = entity["tag"];
            if (hasTag != sol::nullopt)
            {
                newEntity.Tag(entity["tag"]);
            }
            
            // group
            const sol::optional<std::string> hasGroup = entity["group"];
            if (hasGroup != sol::nullopt)
            {
                newEntity.Group(entity["group"]);
            }

            // components
            const sol::optional<sol::table> hasComponents = entity["components"];
            if (hasComponents != sol::nullopt)
            {
                const sol::table components = entity["components"];
                // transform
                const sol::optional<sol::table> hasTransform = components["transform"];
                if (hasTransform != sol::nullopt)
                {
                   
                    const sol::table transform = components["transform"];
                   //get_or assigns the values passed as argument if a value was not specified
                    newEntity.AddComponent<TransformComponent>(
                        glm::vec2(transform["position"]["x"], transform["position"]["y"]),
                        glm::vec2(transform["scale"]["x"].get_or(1.0), transform["scale"]["y"].get_or(1.0)),
                        transform["rotation"].get_or(0)
                        );
                }

                // rigidbody
                const sol::optional<sol::table> hasRigidbody = components["rigidbody"];
                if (hasRigidbody != sol::nullopt)
                {
                    const sol::table rigidbody = components["rigidbody"];
                    newEntity.AddComponent<RigidbodyComponent>(
                        glm::vec2(rigidbody["velocity"]["x"].get_or(0), rigidbody["velocity"]["y"].get_or(0))
                        );
                }

                // Sprite
                sol::optional<sol::table> sprite = entity["components"]["sprite"];
                if (sprite != sol::nullopt)
                {
                    newEntity.AddComponent<SpriteComponent>(
                        entity["components"]["sprite"]["texture_asset_id"],
                        entity["components"]["sprite"]["width"],
                        entity["components"]["sprite"]["height"],
                        entity["components"]["sprite"]["z_index"].get_or(1),
                        entity["components"]["sprite"]["fixed"].get_or(false),
                        entity["components"]["sprite"]["src_rect_x"].get_or(0),
                        entity["components"]["sprite"]["src_rect_y"].get_or(0)
                        );
                }

                // Animation
                sol::optional<sol::table> animation = entity["components"]["animation"];
                if (animation != sol::nullopt)
                {
                    newEntity.AddComponent<AnimationComponent>(
                        entity["components"]["animation"]["num_frames"].get_or(1),
                        entity["components"]["animation"]["speed_rate"].get_or(1)
                        );
                }

                // BoxCollider
                sol::optional<sol::table> collider = entity["components"]["box_collider"];
                if (collider != sol::nullopt)
                {
                    newEntity.AddComponent<BoxColliderComponent>(
                        entity["components"]["box_collider"]["width"],
                        entity["components"]["box_collider"]["height"],
                        glm::vec2(
                            entity["components"]["box_collider"]["offset"]["x"].get_or(0),
                            entity["components"]["box_collider"]["offset"]["y"].get_or(0)
                        )
                        );
                }

                // Health
                sol::optional<sol::table> health = entity["components"]["health"];
                if (health != sol::nullopt)
                {
                    newEntity.AddComponent<HealthComponent>(
                        static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                        );
                }

                // ProjectileEmitter
                sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
                if (projectileEmitter != sol::nullopt)
                {
                    newEntity.AddComponent<ProjectileEmitterComponent>(
                        glm::vec2(
                            entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                            entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                        ),
                        static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                        static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                        static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                        entity["components"]["projectile_emitter"]["should_collide_with_player"].get_or(false)
                        );
                }

                // CameraFollow
                sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
                if (cameraFollow != sol::nullopt)
                {
                    newEntity.AddComponent<CameraFollowComponent>();
                }

                // KeyboardControlled
                sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
                if (keyboardControlled != sol::nullopt)
                {
                    newEntity.AddComponent<KeyboardControlledComponent>(
                        glm::vec2(
                            entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                            entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                        ),
                        glm::vec2(
                            entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                            entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                        ),
                        glm::vec2(
                            entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                            entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                        ),
                        glm::vec2(
                            entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                            entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                        )
                        );
                }
            }
            i++;
        }
    }
}
