#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "collisions.h"
#include "player_object.h"
#include "sound_engine.h"
#include "text.h"

#include <iostream>
#include <string>

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

    ResourceManager::LoadTexture("data/textures/background.png", true, "background");
    ResourceManager::LoadTexture("data/textures/background_menu.png", true, "menu");
    ResourceManager::LoadTexture("data/textures/background_gameWon.png", true, "gameWon");
    ResourceManager::LoadTexture("data/textures/background_gameLost.png", true, "gameLost");
    ResourceManager::LoadTexture("data/textures/froggyjump_tileset.png", true, "froggyjump_tileset");

    const int totalLevels = 8;

    for (int i = 1; i <= totalLevels; i++)
    {
        GameLevel level;
        std::string levelSrc = "data/levels/level" + std::to_string(i) + ".csv";
        level.Load(levelSrc.c_str(), this->Width, this->Height);
        this->Levels.push_back(level);
    }
    
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

}

void Game::Update(float dt)
{
    if (this->State != GAME_ACTIVE)
        return;
    
    // Gravity
    this->SimulatePhysics(dt);

    // Collisions
    this->Collisions(dt);

    Player->UpdateAnimations(dt);

    // Check level beaten and load next
    this->LevelCompleted();
}

void Game::SimulatePhysics(float dt)
{
    float gravity = 980.0f;
    float friction = 10.0f;
    float drag = 8.0f;

    Player->Velocity.y += gravity * dt;

    if (Player->IsOnGround)
        Player->Velocity.x -= Player->Velocity.x * friction * dt;
    else
        Player->Velocity.x -= Player->Velocity.x * drag * dt;

    if (std::abs(Player->Velocity.x) < 15.0f) 
        Player->Velocity.x = 0.0f;

    Player->Position += Player->Velocity * dt;

    Player->IsOnGround = false;
}

void Game::Collisions(float dt)
{
    glm::vec2 playerHalf = Player->CollisionSize * 0.5f;
    // glm::vec2 pSize = Player->CollisionSize;
    glm::vec2 playerCenter;

    // Terrain collisions
    for (GameObject &tile : this->Levels[this->CurrentLevel].Tiles)
    {
        if (checkCollisionAABB(*Player, tile))
        {
            playerCenter = this->GetPlayerCenter(); 
            glm::vec2 tileHalf   = tile.CollisionSize * 0.5f;
            glm::vec2 tileCenter = (tile.Position + tile.CollisionOffset) + tileHalf;

            glm::vec2 diff = playerCenter - tileCenter;
            float overlapX = (playerHalf.x + tileHalf.x) - std::abs(diff.x);
            float overlapY = (playerHalf.y + tileHalf.y) - std::abs(diff.y);

            if (overlapX < overlapY)
            {
                Player->Position.x += (diff.x > 0) ? overlapX : -overlapX;
                Player->Velocity.x = 0;
            }
            else
            {
                if (diff.y < 0)
                {
                    // Colisión desde arriba (suelo)
                    Player->Position.y -= overlapY;
                    Player->IsOnGround = true;
                }
                else
                {
                    // Colisión desde abajo (techo)
                    Player->Position.y += overlapY;
                }
                Player->Velocity.y = 0;
            }
        }
    }

    playerCenter = this->GetPlayerCenter();

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
    if (playerCenter.x - playerHalf.x < 0.0f) {
        // Left
        Player->Position.x = -Player->CollisionOffset.x;
        Player->Velocity.x = 0.0f;
    } 
    else if (playerCenter.x + playerHalf.x > this->Width) {
        // Right
        Player->Position.x = this->Width - Player->CollisionOffset.x - Player->CollisionSize.x;
        Player->Velocity.x = 0.0f;
    }

    // Bottom
    if (playerCenter.y + playerHalf.y > this->Height) {
        HandlePlayerDeath();
    }
}

glm::vec2 Game::GetPlayerCenter() {
    return Player->Position + Player->CollisionOffset + (Player->CollisionSize * 0.5f);
}

void Game::HandlePlayerDeath() {
    Player->Death();
    Sound.StartSound(SFX_FAIL);
    if (Player->Lives <= 0) this->State = GAME_OVER;
    else Player->Reset(this->Levels[this->CurrentLevel].PlayerData.Position);
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
        // for (int i = 0; i < 8; i++) {
        //     if (this->Keys[GLFW_KEY_KP_1 + i]) {
        //         this->CurrentLevel = i;
        //         Player->Reset(this->Levels[i].PlayerData.Position);
        //     }
        // }

        // if (this->Keys[GLFW_KEY_KP_0])
        // {
        //     Player->Lives++;
        //     Player->Position = this->Levels[this->CurrentLevel].PlayerData.Position;
        //     Player->Velocity = glm::vec2(0.0f, 0.0f);
        // }
    }

    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
        }
    }

    if (this->State == GAME_OVER)
    {
        if (this->Keys[GLFW_KEY_F])
        {
            Player->Lives = 3;
            Player->Reset(this->Levels[this->CurrentLevel].PlayerData.Position);
            this->State = GAME_ACTIVE;
        }
    }

    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_R])
        {
            this->Reset();
        }
    }
    
}

void Game::Render(int fps)
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
            LivesStr = "FPS: " + std::to_string(fps);
            textPos =  glm::vec2(this->Width - 125.0f, 5.0f);
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

void Game::Reset()
{
    for (GameLevel &level : this->Levels)
    {
        level.Reset();
    }
    Player->Lives = 3;
    this->CurrentLevel = 0;
    Player->Reset(this->Levels[this->CurrentLevel].PlayerData.Position);
    this->State = GAME_ACTIVE;
}
