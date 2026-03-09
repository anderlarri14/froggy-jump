#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"

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

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("frog").Use().SetInteger("image", 0);
    ResourceManager::GetShader("frog").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(ResourceManager::GetShader("frog"));

    ResourceManager::LoadTexture("data/textures/background.jpg", false, "background");
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
    this->CurrentLevel = 2;


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
        ResourceManager::GetTexture("background"),
        glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height)
    );

    if (this->State == GAME_ACTIVE)
    {
        this->Levels[this->CurrentLevel].Draw(*Renderer);
    }
    
}
