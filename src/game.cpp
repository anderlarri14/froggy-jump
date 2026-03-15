#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "collisions.h"
#include "player_object.h"

#include <iostream>

SpriteRenderer  *Renderer;
PlayerObject *Player;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete Player;
}

void Game::Init()
{
    ResourceManager::LoadShader("data/shaders/sprite.vs", "data/shaders/sprite.fs", nullptr, "frog");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("frog").Use().SetInteger("image", 0);
    ResourceManager::GetShader("frog").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(ResourceManager::GetShader("frog"));

    ResourceManager::LoadTexture("data/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("data/textures/gameWon.png", true, "gameWon");
    ResourceManager::LoadTexture("data/textures/gameLost.png", true, "gameLost");
    ResourceManager::LoadTexture("data/textures/frog.png", true, "frog");
    ResourceManager::LoadTexture("data/textures/dirt.png", true, "dirt");
    ResourceManager::LoadTexture("data/textures/grass.png", true, "grass");
    ResourceManager::LoadTexture("data/textures/apple.png", true, "apple");

    GameLevel one, two, three; 
    one.Load("data/levels/level1.csv", this->Width, this->Height);
    two.Load("data/levels/level2.csv", this->Width, this->Height);
    three.Load("data/levels/level3.csv", this->Width, this->Height);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->CurrentLevel = 0;

    float playerSpeed = 1750.0f;
    float jumpHeight = 600.0f;

    Player = new PlayerObject(
        this->Levels[this->CurrentLevel].PlayerData.Position,
        this->Levels[this->CurrentLevel].PlayerData.Size,
        this->Levels[this->CurrentLevel].PlayerData.Sprite,
        glm::vec2(0.0f, 0.0f),
        playerSpeed,
        jumpHeight
    );
    std::cout << "PlayerData is: (" << Player->Position.x <<","<< Player->Position.y <<") (" << Player->Size.x <<","<< Player->Size.y <<") "<< Player->Sprite.ID << std::endl;

}

void Game::Update(float dt)
{
    // Gravity
    this->SimulatePhysics(dt);

    // Collisions
    this->Collisions(dt);

    // Check level beaten and load next
    this->LevelCompleted();
}

void Game::SimulatePhysics(float dt)
{
    float gravity = 980.0f;
    float friction = 10.0f;
    float drag = 8.0f;

    // 1. Aplicar Gravedad a la velocidad
    Player->Velocity.y += gravity * dt;


    if (Player->IsOnGround)
        Player->Velocity.x -= Player->Velocity.x * friction * dt;
    else
        Player->Velocity.x -= Player->Velocity.x * drag * dt;

    Player->Position += Player->Velocity * dt;

    Player->IsOnGround = false;
}

void Game::Collisions(float dt)
{
    // Terrain collisions
    for (GameObject &tile : this->Levels[this->CurrentLevel].Tiles)
    {
        if (checkCollisionAABB(*Player, tile))
        {
            float overlapX = (Player->Size.x / 2 + tile.Size.x / 2) - std::abs((Player->Position.x + Player->Size.x / 2) - (tile.Position.x + tile.Size.x / 2));
            float overlapY = (Player->Size.y / 2 + tile.Size.y / 2) - std::abs((Player->Position.y + Player->Size.y / 2) - (tile.Position.y + tile.Size.y / 2));

            if (overlapX < overlapY)
            {
                if (Player->Position.x < tile.Position.x)
                {
                    Player->Position.x -= overlapX; // Left colision
                }
                else
                {
                    Player->Position.x += overlapX; // Right colision
                }
                Player->Velocity.x = 0;
            }
            else
            {
                if (Player->Position.y < tile.Position.y)
                {
                    Player->Position.y -= overlapY; // Up colision
                    Player->IsOnGround = true;
                }
                else
                {
                    Player->Position.y += overlapY; // Down colision
                }
                Player->Velocity.y = 0;
            }
        }
    }

    // Collectable collisions
    for (GameObject &collectable : this->Levels[this->CurrentLevel].Collectables)
    {
        if (collectable.IsCollected)
            continue;

        if (checkCollisionAABB(*Player, collectable))
        {
            collectable.IsCollected = true;
            std::cout << "Collision with apple :D" << std::endl;
        }
    }

    // Out of bounds collisions
    if (Player->Position.x < 0.0f)
    {
        Player->Position.x = 0.0f;
        Player->Velocity.x = 0.0f;
    } 
    else if (Player->Position.x + Player->Size.x > this->Width)
    {
        Player->Position.x = this->Width - Player->Size.x;
        Player->Velocity.x = 0.0f;
    }
    else if (Player->Position.y < 0.0f)
    {
        Player->Position.y = 0.0f;
        Player->Velocity.y = 0.0f;
    }
    else if (Player->Position.y > this->Height)
    {
        /* FALL DEATH LOGIC */
        if (Player->Lives == 0)
            this->State = GAME_OVER;
        
        
        Player->Death();
        Player->Reset(this->Levels[this->CurrentLevel].PlayerData.Position);
    }
    
}

void Game::ProcessInput(float dt)
{
   if (this->State == GAME_ACTIVE)
    {
        // Player movement
        if (this->Keys[GLFW_KEY_A])
        {
            Player->Velocity.x -= Player->Speed * dt;
        }
        if (this->Keys[GLFW_KEY_D])
        {
            Player->Velocity.x += Player->Speed * dt;
        }
        if (this->Keys[GLFW_KEY_W] && Player->IsOnGround)
        {
            Player->Velocity.y = -Player->JumpHeight;
        }



        // Debug tools
        if (this->Keys[GLFW_KEY_KP_1])
        {
            this->CurrentLevel = 0;
        }
        if (this->Keys[GLFW_KEY_KP_2])
        {
            this->CurrentLevel = 1;
        }
        if (this->Keys[GLFW_KEY_KP_3])
        {
            this->CurrentLevel = 2;
        }
        if (this->Keys[GLFW_KEY_KP_0])
        {
            Player->Position = this->Levels[this->CurrentLevel].PlayerData.Position;
            Player->Velocity = glm::vec2(0.0f, 0.0f);
        }
    }
}

void Game::Render()
{
    switch (this->State)
    {
    case GAME_ACTIVE:
        Renderer->DrawSprite(
            ResourceManager::GetTexture("background"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height)
        );

        this->Levels[this->CurrentLevel].Draw(*Renderer);

        Player->Draw(*Renderer);
        break;

    case GAME_WIN:
        Renderer->DrawSprite(
            ResourceManager::GetTexture("gameWon"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height)
        );
        break;
    case GAME_OVER:
        Renderer->DrawSprite(
            ResourceManager::GetTexture("gameLost"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height)
        );
        break;
    default:
        break;
    }
}

void Game::LevelCompleted()
{
    if (this->State == GAME_WIN)
        return;
    
    if (this->Levels[this->CurrentLevel].IsCompleted())
    {
        if (this->CurrentLevel < this->Levels.size() - 1)
        {
            this->CurrentLevel++;
            Player->Reset(this->Levels[this->CurrentLevel].PlayerData.Position);
        }
        else
        {
            this->State = GAME_WIN;
        }
        
    }
}
