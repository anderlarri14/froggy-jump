#include "game_level.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "resource_manager.h"


void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    this->Tiles.clear();
    std::cout << "Loading level: " << file << std::endl;
    std::ifstream levelFile(file);
    std::string line;
    std::vector<std::vector<int>> tileData;

    if (levelFile)
    {
        while (std::getline(levelFile, line))
        {
            std::istringstream sstream(line);
            std::string token;
            std::vector<int> row;

            while (std::getline(sstream, token, ','))
            {
                row.push_back(std::stoi(token));
            }
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->Init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Init(const std::vector<std::vector<int>> &tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    unsigned int yTiles = tileData.size();
    unsigned int xTiles = tileData[0].size();

    float tileHeight   = levelHeight / yTiles;
    float tileWidth    = levelWidth / xTiles;

    std::string textureName = "froggyjump_tileset";
    for (unsigned int y = 0; y < yTiles; y++)
    {
        for (unsigned int x = 0; x < xTiles; x++)
        {
            glm::vec2 pos(x * tileWidth, y * tileHeight);
            glm::vec2 size(tileWidth, tileHeight);
            
            switch(tileData[y][x]) 
            {
                case AIR_TYPE:
                    continue;
                case FROG_TYPE:
                    this->PlayerData = GameObject(pos, size, ResourceManager::GetTexture(textureName), tileData[y][x]);
                    this->PlayerData.CollisionSize = glm::vec2(size.x * 0.6f, size.y * 0.6f);
                    this->PlayerData.CollisionOffset = glm::vec2(size.x * 0.2f, size.y * 0.4f);
                    continue;
                case APPLE_TYPE:
                {
                    GameObject apple(pos, size, ResourceManager::GetTexture(textureName), tileData[y][x]);
                    apple.CollisionSize   = glm::vec2(size.x * 0.5f, size.y * 0.5f);
                    apple.CollisionOffset = glm::vec2(size.x * 0.25f, size.y * 0.25f);
                    this->Collectables.push_back(apple);
                    continue;
                }
                default:
                    break;
            }
            this->Tiles.push_back(GameObject(pos, size, ResourceManager::GetTexture(textureName), tileData[y][x]));
        }
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Tiles)
    {
        tile.Draw(renderer);
    }

    for (GameObject &collectable : this->Collectables)
    {
        if (!collectable.IsCollected)
            collectable.Draw(renderer);
    }
}

bool GameLevel::IsCompleted()
{
    for (GameObject &collectable : this->Collectables)
    {
        if (!collectable.IsCollected)
            return false;
    }
    return true;
}
