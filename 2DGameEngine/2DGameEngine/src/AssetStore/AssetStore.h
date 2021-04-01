#pragma once

#include <map>
#include <string>
#include <SDL.h>

class AssetStore
{
public:
	AssetStore();
	~AssetStore();

	void AddTexture(SDL_Renderer* renderer, const std::string& assetName, const std::string& filePath);
	SDL_Texture* GetTexture(const std::string& assetName) const;

	void ClearAssets();
private:
	std::map<std::string, SDL_Texture*> m_textures;
};

