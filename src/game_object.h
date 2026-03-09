#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

enum GameObjectTypes{
    AIR_TYPE = -1,
    DIRT_TYPE = 0,
    GRASS_TYPE = 1,
    FROG_TYPE = 2,
    APPLE_TYPE = 3
};


class GameObject
{
public:
    glm::vec2 Position, Size;
    int TileType;

    Texture2D Sprite;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite);
    void Draw(SpriteRenderer &renderer);

private:


};
#endif 
