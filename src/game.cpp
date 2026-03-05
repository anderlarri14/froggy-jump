#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

SpriteRenderer  *Renderer;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    ResourceManager::LoadShader("data/shaders/sprite.vs", "data/shaders/sprite.fs", nullptr, "frog");

    //TODO: DUDA Why not (float)?
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("frog").Use().SetInteger("image", 0);
    ResourceManager::GetShader("frog").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(ResourceManager::GetShader("frog"));

    ResourceManager::LoadTexture("data/textures/frog64x64.png", true, "frog");
}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{
    Renderer->DrawSprite(
        ResourceManager::GetTexture("frog"), 
        glm::vec2(200.0f, 200.0f), glm::vec2(64.0f, 64.0f)
    );
}
