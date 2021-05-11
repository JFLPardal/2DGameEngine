#pragma once

#include <memory>

class Registry;
class AssetStore;
struct SDL_Renderer;
namespace sol { class state; }

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader() = default;

	void LoadLevel(unsigned int levelToLoad, const std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, sol::state& lua);
};