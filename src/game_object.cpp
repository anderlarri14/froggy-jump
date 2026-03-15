#include "game_object.h"


GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Sprite(), TileType(-1), IsCollected(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, int tileType)
    : Position(pos), Size(size), Sprite(sprite), TileType(tileType), IsCollected(false) { }

void GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size);
}
