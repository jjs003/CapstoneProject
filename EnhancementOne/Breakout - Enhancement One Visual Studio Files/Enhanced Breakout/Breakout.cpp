/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This file contains the main entry point for the Breakout game.
** It sets up the OpenGL context, initializes the game, and runs
** the main game loop.
******************************************************************/


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

#include <iostream>

// --- Constants ---
const unsigned int SCREEN_WIDTH = 800;  // Width of the application window.
const unsigned int SCREEN_HEIGHT = 600; // Height of the application window.

// --- Global Variables ---
Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT); // Game instance.

// --- GLFW function declarations ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


int main(int argc, char* argv[])
{
    // Initialize GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // Use OpenGL 3.3 Core Profile.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on macOS.
#endif

    glfwWindowHint(GLFW_RESIZABLE, false);  // Disable resizing for consistent behavior.

    // Create GLFW window.
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD to load OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Register callback functions.
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Configure OpenGL settings.
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the game.
    Breakout.Init();

    // Timing variables for frame management.
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Main game loop (frame).
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time (time elapsed between frames).
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // Poll for input events (from the user).
        Breakout.ProcessInput(deltaTime);

        // Update the game state.
        Breakout.Update(deltaTime);

        // Render the current frame.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set background color to black.
        glClear(GL_COLOR_BUFFER_BIT);          // Clear the screen.
        Breakout.Render();

        // Swap the front and back buffers.
        glfwSwapBuffers(window);
    }

    // Clean up resources.
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}

// --- Callback Functions ---

// Handles keyboard input events.
// Parameters:
// - window: The GLFW window receiving the input.
// - key: The key that was pressed or released.
// - scancode: The platform-specific scancode of the key.
// - action: Action performed (GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT).
// - mode: Bitfield describing additional modifier keys (e.g., Shift, Control).
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Close the application when the Escape key is pressed.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    
    // Handle other keys.
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            Breakout.Keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            Breakout.Keys[key] = false;
        }
    }
}


// Handles resizing of the application window.
// Parameters:
// - window: The GLFW window being resized.
// - width: New width of the window.
// - height: New height of the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

