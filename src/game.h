#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_OVER
};

class Game
{
public:
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    Game(unsigned int width, unsigned int height);
    ~Game();
    void Init();

    std::vector<GameLevel> Levels;
    unsigned int CurrentLevel;

    // Game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void SimulatePhysics(float dt);
    void Collisions(float dt);
    void Render();
    void LevelCompleted();

};
#endif
