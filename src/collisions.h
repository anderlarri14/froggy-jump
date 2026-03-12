#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "game_object.h"

#include <iostream>

bool checkCollisionAABB(GameObject &A, GameObject &B)
{
    if (A.Position.x + A.Size.x > B.Position.x && B.Position.x + B.Size.x > A.Position.x)
    {
        if (A.Position.y + A.Size.y > B.Position.y && B.Position.y + B.Size.y > A.Position.y)
            return true;
    }

    return false;
}

#endif