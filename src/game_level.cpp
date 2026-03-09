#include "game_level.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "resource_manager.h"


void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    this->Tiles.clear();
    std::cout << "Loading level csv..." << std::endl;
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

    std::string textureName;
    for (unsigned int y = 0; y < yTiles; y++)
    {
        for (unsigned int x = 0; x < xTiles; x++)
        {
            glm::vec2 pos(x * tileWidth, y * tileHeight);
            glm::vec2 size(tileHeight, tileWidth);
            
            switch(tileData[y][x]) 
            {
                case AIR_TYPE:
                    continue;
                case DIRT_TYPE:
                    textureName = "dirt";
                    break;
                case GRASS_TYPE:
                    textureName = "grass";
                    break;
                case FROG_TYPE:
                    textureName = "frog";
                    break;
                case APPLE_TYPE:
                    textureName = "apple";
                    break;
                default:
                    continue;
            }
            this->Tiles.push_back(GameObject(pos, size, ResourceManager::GetTexture(textureName)));
        }
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Tiles)
    {
        tile.Draw(renderer);
    }
    
}
