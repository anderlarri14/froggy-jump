#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>
#include <glm/glm.hpp>

#include "game_object.h"



class GameLevel
{
public:
    std::vector<GameObject> Tiles;
    GameObject PlayerData;
    std::vector<GameObject> Collectables;
    
    GameLevel() {};
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    void Draw(SpriteRenderer &renderer);
    void Reset();
    bool IsCompleted();
    
private:
    void Init(const std::vector<std::vector<int>> &tileData, unsigned int levelWidth, unsigned int levelHeight);

};



#endif 
