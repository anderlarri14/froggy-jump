#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "game_object.h"

#include <iostream>

bool checkCollisionAABB(GameObject &A, GameObject &B)
{
    float AColisionPosX = A.Position.x + A.CollisionOffset.x;
    float AColisionPosY = A.Position.y + A.CollisionOffset.y;

    float BColisionPosX = B.Position.x + B.CollisionOffset.x;
    float BColisionPosY = B.Position.y + B.CollisionOffset.y;

    if (AColisionPosX + A.CollisionSize.x > BColisionPosX && BColisionPosX + B.CollisionSize.x > AColisionPosX)
    {
        if (AColisionPosY + A.CollisionSize.y > BColisionPosY && BColisionPosY + B.CollisionSize.y > AColisionPosY)
            return true;
    }
    return false;
}

#endif
