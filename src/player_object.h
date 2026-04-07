#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "game_object.h"

// Matches with tilemap columns
enum PlayerAnimationState
{
    PLAYER_ANIM_IDLE = 1,
    PLAYER_ANIM_WALK = 2,
    PLAYER_ANIM_JUMP = 3
};

enum PlayerFacing
{
    PLAYER_FACING_LEFT = 0,
    PLAYER_FACING_RIGHT = 1
};

class PlayerObject : public GameObject
{
public:
    float JumpHeight;
    glm::vec2 Velocity;
    bool IsOnGround;
    float Speed;
    int Lives;

    PlayerObject();
    PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity, float playerSpeed, float jumpHeight);
    
    void Reset(glm::vec2 startPos);
    void Death();
    void UpdateAnimations(float dt);
    void Draw(SpriteRenderer &renderer);

private:
    int PlayerAnimState = PLAYER_ANIM_IDLE;
    int currentFrame = 0;
    int totalFrames = 4; 
    float timePerFrame = 0.25f;
    float animationCounter = 0.0f;
    PlayerFacing Facing = PLAYER_FACING_RIGHT; 
    float UVMultiplier = -1.0f;
    glm::vec2 GetUVOffsetFromTileType();

};


#endif
