#include "player_object.h"

#include <iostream>

PlayerObject::PlayerObject() 
    : GameObject(), Velocity(glm::vec2(0.0f, 0.0f)), IsOnGround(false), Speed(0.0f), JumpHeight(0.0f) { }

PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity, float speed, float jumpHeight)
    : GameObject(pos, size, sprite, FROG_TYPE), Velocity(velocity), IsOnGround(false), Speed(speed), JumpHeight(jumpHeight) { }


void PlayerObject::Reset(glm::vec2 startPos)
{
    this->Position = startPos;
    this->Velocity = glm::vec2(0.0f, 0.0f);
    this->IsOnGround = false;
}

void PlayerObject::Death()
{
    this->Lives--;
    std::cout << "Number of lives: "<< this->Lives << std::endl;
}