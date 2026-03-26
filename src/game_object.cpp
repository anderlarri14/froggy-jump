#include "game_object.h"

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), CollisionOffset(0.0f, 0.0f), CollisionSize(1.0f, 1.0f),
    Sprite(), TileType(-1), IsCollected(false), TileSizeUV(1.0f/4.0f, 1.0f/6.0f) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, int tileType)
    : Position(pos), Size(size), CollisionOffset(0.0f), CollisionSize(size),
    Sprite(sprite), TileType(tileType), IsCollected(false), TileSizeUV(1.0f/4.0f, 1.0f/6.0f) { }

void GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size,
                        0.0f, glm::vec3(1.0f),
                        this->TileSizeUV,
                        this->GetUVOffsetFromTileType());
}

glm::vec2 GameObject::GetUVOffsetFromTileType()
{
    int row = 0, col = 0;

    switch (this->TileType)
    {
    case DIRT_TYPE:
        col = 0;
        row = 0;
        break;
    case GRASS_TYPE:
        col = 0;
        row = 1;
        break;
    case FROG_TYPE:
        col = 1;
        row = 0;
        break;
    case APPLE_TYPE:
        col = 0;
        row = 2;
        break;
    default:
        break;
    }
    return glm::vec2(this->TileSizeUV.x * col, this->TileSizeUV.y * row);
}

