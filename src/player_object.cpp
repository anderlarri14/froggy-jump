#include "player_object.h"

#include <iostream>

PlayerObject::PlayerObject() 
    : GameObject(), Velocity(glm::vec2(0.0f, 0.0f)), IsOnGround(false), Speed(0.0f), JumpHeight(0.0f), Lives(3) { }

PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity, float speed, float jumpHeight)
    : GameObject(pos, size, sprite, FROG_TYPE), Velocity(velocity), IsOnGround(false), Speed(speed), JumpHeight(jumpHeight), Lives(3) { }


void PlayerObject::Reset(glm::vec2 startPos)
{
    this->Position = startPos;
    this->Velocity = glm::vec2(0.0f, 0.0f);
    this->IsOnGround = false;
}

void PlayerObject::Death()
{
    this->Lives--;
}

void PlayerObject::UpdateAnimations(float dt)
{

    if (std::abs(this->Velocity.x) > 10.0f) 
    {
        if (this->Velocity.x < 0.0f && this->Facing != PLAYER_FACING_LEFT)
        {
            this->Facing = PLAYER_FACING_LEFT;
            this->UVMultiplier = 1.0f;  // Sprite original (Izquierda)
        }
        else if (this->Velocity.x > 0.0f && this->Facing != PLAYER_FACING_RIGHT)
        {
            this->Facing = PLAYER_FACING_RIGHT;
            this->UVMultiplier = -1.0f; // Volteado (Derecha)
        }
    }

    PlayerAnimationState newState;

    if (!this->IsOnGround) 
    {
        newState = PLAYER_ANIM_JUMP;
    } 
    else if (std::abs(this->Velocity.x) > 0.1f)
    {
        newState = PLAYER_ANIM_WALK;
    } 
    else 
    {
        newState = PLAYER_ANIM_IDLE;
    }


    if (this->PlayerAnimState != newState)
    {
        this->PlayerAnimState = newState;
        this->currentFrame = 0;
        this->animationCounter = 0.0f;

        switch (this->PlayerAnimState)
        {
            case PLAYER_ANIM_IDLE: 
                this->totalFrames = 2;
                this->timePerFrame = 0.3f;
                break;
            case PLAYER_ANIM_WALK: 
                this->totalFrames = 5; 
                this->timePerFrame = 0.1f;
                break; 
            case PLAYER_ANIM_JUMP: 
                this->totalFrames = 4;
                this->timePerFrame = 0.15f;
                break; 
        }
    }


    this->animationCounter += dt;

    if (this->animationCounter >= this->timePerFrame)
    {
        this->animationCounter -= this->timePerFrame;
        
        if (this->PlayerAnimState == PLAYER_ANIM_JUMP)
        {
            if (this->currentFrame < (this->totalFrames - 1))
            {
                this->currentFrame++;
            }
        }
        else
        {
            this->currentFrame = (this->currentFrame + 1) % this->totalFrames;
        }
    }
}

void PlayerObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size,
                        0.0f, glm::vec3(1.0f),
                        this->TileSizeUV * glm::vec2(this->UVMultiplier, 1.0f),
                        this->GetUVOffsetFromTileType());
}

glm::vec2 PlayerObject::GetUVOffsetFromTileType()
{
    int row = this->currentFrame;
    int col = this->PlayerAnimState;

    float uOffset = this->TileSizeUV.x * col;
    float vOffset = this->TileSizeUV.y * row;


    if (this->UVMultiplier < 0.0f)
    {
        uOffset += this->TileSizeUV.x;
    }

    return glm::vec2(uOffset, vOffset);
}
