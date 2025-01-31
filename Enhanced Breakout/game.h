/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This header file defines the `Game` class, which manages the
** game state, logic, and rendering. It also defines game-related
** constants, enumerations, and data structures.
******************************************************************/


#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"


// --- Enumerations ---

// Represents the current state of the game.
enum GameState {
    GAME_ACTIVE,  // The game is actively being played.
    GAME_MENU,    // The game is displaying the menu screen.
    GAME_WIN,     // The player has won the game.
    GAME_OVER     // The player ran out of lives.
};

// Represents the four possible (collision) directions
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// --- Type Definitions ---

// Represents collision data:
// - `bool`: Whether a collision occurred.
// - `Direction`: The direction of the collision.
// - `glm::vec2`: The vector difference between the object's center and the closest collision point.
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>


// --- Constants ---

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);

// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

// Initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY(0.0f, -500.0f);

// Radius of the ball object
const float BALL_RADIUS = 12.5f;

// --- Game Class ---

// The `Game` class holds all game-related state and functionality.
// It combines all game-related data into a single, manageable class.
class Game
{
public:
    // --- Game State ---
    GameState               State;                // Current state of the game.
    bool                    Keys[1024];           // Stores the state of each key (pressed/released).
    bool                    KeysProcessed[1024];  // Keeps track of key presses that have been processed
    unsigned int            Width, Height;        // Dimensions of the game window.
    std::vector<GameLevel>  Levels;               // Stores all game levels.
    unsigned int            Level;                // Current game level index.
    unsigned int            Lives;                // Keeps track of the player's lives

    // --- Constructor/Destructor ---
    Game(unsigned int width, unsigned int height);  // Initializes game with the specified dimensions.
    ~Game();  // Destructor for cleaning up resources.

    // --- Game Functions ---

    // Initializes game state, including loading shaders, textures, and levels.
    void Init();

    // Processes user input based on the time elapsed (delta time).
    const void ProcessInput(float dt);

    // Updates the game state based on the time elapsed between frames (delta time).
    void Update(float dt);

    // Renders the current game frame.
    void Render();

    // Handles all collisions between game objects.
    void DoCollisions();

    // --- Reset Functions ---
    
    // Resets the current level to its initial state.
    void ResetLevel();

    // Resets the player's position and state to its initial configuration.
    void ResetPlayer();
};

#endif  // GAME_H

