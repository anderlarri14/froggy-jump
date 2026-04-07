#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

#include <iostream>
#include <filesystem>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 768;

Game FroggyJump(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[])
{
    if (argc > 0) {
        std::filesystem::path exePath = std::filesystem::absolute(argv[0]).parent_path();
        std::filesystem::current_path(exePath);
    }

    // Game window config
    // --------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Froggy Jump", nullptr, nullptr);
    glfwMakeContextCurrent(window);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowPos(window, (mode->width/2) - SCREEN_WIDTH/2, (mode->height/2) - SCREEN_HEIGHT/2);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    FroggyJump.Init();


    float deltaTime = 0.0f;
    float currentFrame = 0.0f;
    float lastFrame = 0.0f;
    
    float targetFPS = 60.0;
    float timePerFrame = 1.0 / targetFPS;

    float lastFpsUpdate = 0.0f;
    float fpsUpdateInterval = 1.0f;
    int frameCount = 0;
    int currentFps = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED))
        {
            lastFrame = glfwGetTime();
            continue;
        }

        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        // FPS limit
        if (deltaTime < timePerFrame)
            continue; 
        
        lastFrame = currentFrame;

        lastFpsUpdate += deltaTime;
        frameCount++;
        if (lastFpsUpdate >= fpsUpdateInterval)
        {
            currentFps = (int)((frameCount / lastFpsUpdate) + 0.5f);
            
            lastFpsUpdate = 0.0f;
            frameCount = 0;
        }

        // Manage user input
        // -----------------
        FroggyJump.ProcessInput(deltaTime);

        // Update game state
        // -----------------
        FroggyJump.Update(deltaTime);

        // Render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        FroggyJump.Render(currentFps);

        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            FroggyJump.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            FroggyJump.Keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}