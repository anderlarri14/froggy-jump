#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

enum GameObjectTypes{
    AIR_TYPE = -1,
    DIRT_TYPE = 0,
    GRASS_TYPE = 4,
    FROG_TYPE = 1,
    APPLE_TYPE = 8
};


class GameObject
{
public:
    glm::vec2 Position, Size;
    int TileType;
    bool IsCollected;

    Texture2D Sprite;
    glm::vec2 CollisionOffset, CollisionSize;
    glm::vec2 TileSizeUV;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, int tileType);
    void Draw(SpriteRenderer &renderer);

private:
    glm::vec2 GetUVOffsetFromTileType();

};
#endif 
