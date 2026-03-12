#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "game_object.h"

class PlayerObject : public GameObject
{
private:
    
public:
    float JumpHeight;
    glm::vec2 Velocity;
    bool IsOnGround;
    float Speed;
    
    PlayerObject();
    PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity, float playerSpeed, float jumpHeight);
    
    void Reset(glm::vec2 startPos);
};


#endif
