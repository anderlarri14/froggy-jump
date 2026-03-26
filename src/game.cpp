#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "collisions.h"
#include "player_object.h"
#include "sound_engine.h"
#include "text.h"

#include <iostream>

SpriteRenderer  *Renderer;
SoundEngine Sound;
Text *UIText;
PlayerObject *Player;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete UIText;
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
    ResourceManager::LoadTexture("data/textures/menu.png", true, "menu");
    ResourceManager::LoadTexture("data/textures/gameWon.png", true, "gameWon");
    ResourceManager::LoadTexture("data/textures/gameLost.png", true, "gameLost");
    ResourceManager::LoadTexture("data/textures/froggyjump_tileset.png", true, "froggyjump_tileset");

    GameLevel one, two, three; 
    one.Load("data/levels/level1.csv", this->Width, this->Height);
    two.Load("data/levels/level2.csv", this->Width, this->Height);
    three.Load("data/levels/level3.csv", this->Width, this->Height);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->CurrentLevel = 0;

    float volume = 0.5f;
    Sound.Init(volume);
    Sound.LoadSound("data/sound/music/EVRetro-MusicLoop.wav", true);
    Sound.LoadSound("data/sound/sfx/jump.wav");
    Sound.LoadSound("data/sound/sfx/acquire.wav");
    Sound.LoadSound("data/sound/sfx/fail.wav");
    Sound.StartSound(MUSIC);

    UIText = new Text();

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
    Player->CollisionSize = this->Levels[this->CurrentLevel].PlayerData.CollisionSize;
    Player->CollisionOffset = this->Levels[this->CurrentLevel].PlayerData.CollisionOffset;
    std::cout << "PlayerData is: (" << Player->Position.x <<","<< Player->Position.y <<") (" << Player->Size.x <<","<< Player->Size.y <<") "<< Player->Sprite.ID << std::endl;

}

void Game::Update(float dt)
{
    if (this->State != GAME_ACTIVE)
        return;
    
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
    // Player collision data
    glm::vec2 playerCollision = Player->Position + Player->CollisionOffset;
    glm::vec2 playerHalf   = Player->CollisionSize * 0.5f;
    glm::vec2 playerCenter = playerCollision + playerHalf;

    // Terrain collisions
    for (GameObject &tile : this->Levels[this->CurrentLevel].Tiles)
    {
        if (checkCollisionAABB(*Player, tile))
        {
            // Tile collision data
            glm::vec2 tileCollision = tile.Position + tile.CollisionOffset;
            glm::vec2 tileHalf   = tile.CollisionSize * 0.5f;
            glm::vec2 tileCenter = tileCollision + tileHalf;

            float overlapX = (playerHalf.x + tileHalf.x) - std::abs(playerCenter.x - tileCenter.x);
            float overlapY = (playerHalf.y + tileHalf.y) - std::abs(playerCenter.y - tileCenter.y);

            if (overlapX < overlapY)
            {
                if (playerCenter.x < tileCenter.x)
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
                if (playerCenter.y < tileCenter.y)
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
            playerCenter = (Player->Position + Player->CollisionOffset) + (Player->CollisionSize * 0.5f);
        }
    }

    // Collectable collisions
    for (GameObject &collectable : this->Levels[this->CurrentLevel].Collectables)
    {
        if (!collectable.IsCollected && checkCollisionAABB(*Player, collectable))
        {
            collectable.IsCollected = true;
            Sound.StartSound(SFX_ACQUIRE);
        }
    }

    // Out of bounds collisions
    if (Player->Position.x + Player->CollisionOffset.x < 0.0f)
    {
        Player->Position.x = -Player->CollisionOffset.x;
        Player->Velocity.x = 0.0f;
    } 
    else if (Player->Position.x + Player->Size.x > this->Width)
    {
        Player->Position.x = this->Width - Player->CollisionOffset.x - Player->CollisionSize.x;
        Player->Velocity.x = 0.0f;
    }
    
    if (Player->Position.y + Player->CollisionOffset.y < 0.0f)
    {
        Player->Position.y = -Player->CollisionOffset.y;
        Player->Velocity.y = 0.0f;
    }
    
    if (Player->Position.y + Player->CollisionOffset.y > this->Height)
    {
        /* FALL DEATH LOGIC */
        Player->Death();
        Sound.StartSound(SFX_FAIL);

        if (Player->Lives <= 0)
        {
            this->State = GAME_OVER;
        }
        else
        {
            Player->Reset(this->Levels[this->CurrentLevel].PlayerData.Position);
        }  
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
            Sound.StartSound(SFX_JUMP);
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

    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
        }
    }
    
}

void Game::Render()
{
    switch (this->State)
    {
        case GAME_MENU:
            Renderer->DrawSprite(
                ResourceManager::GetTexture("menu"),
                glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height)
            );
            break;

        case GAME_ACTIVE:
        {

            Renderer->DrawSprite(
                ResourceManager::GetTexture("background"),
                glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height)
            );
            this->Levels[this->CurrentLevel].Draw(*Renderer);
            Player->Draw(*Renderer);
            
            // Draw UI text
            std::string LivesStr = "Player Lives: " + std::to_string(Player->Lives);
            glm::vec2 textPos =  glm::vec2(5.0f, 5.0f);
            glm::vec4 textColor = glm::vec4(1.0f, 1.0f, 1.0f, 4.0f);
            float size = 1.75f;
            UIText->Draw(LivesStr.c_str(), textPos, textColor, size);
            LivesStr = "Level: " + std::to_string(this->CurrentLevel + 1);
            textPos =  glm::vec2(5.0f, 40.0f);
            UIText->Draw(LivesStr.c_str(), textPos, textColor, size);
            
            break;
        }
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
    {
        UIText->Clear();
        return;
    }
    
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
