/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This file defines the implementation of the `Game` class.
** It includes functions for initializing game resources, processing
** input, updating the game state, rendering, and managing collisions.
******************************************************************/


#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include <iostream>
#include "text_renderer.h"
#include "high_score_DB.h"

// --- Global Variables ---
// Game-related render objects.
SpriteRenderer* Renderer;            // Sprite renderer for drawing 2D objects
GameObject* Player;                  // Player's paddle object
BallObject* Ball;                    // Ball object
ParticleGenerator* Particles;        // Particle generator for ball effects
TextRenderer* Text;                  // Text renderer for displaying text
HighScoreDB* db;                     // Database handler for high scores

// --- Game Class Implementation ---

// Constructor: Initializes game state, width, height, and other variables.
Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0), Lives(3), levelCompletionTime()
{

}

// Destructor: Cleans up dynamically allocated game objects.
Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Text;
    delete db;
}

// Initializes all the game objects, resources, shaders, and levels.
void Game::Init()
{   
    // --- Load Shaders ---
    // Load vertex and fragment shaders for sprite rendering and particles.
    ResourceManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("../shaders/particle.vs", "../shaders/particle.fs", nullptr, "particle");

    // --- Configure shaders ---
    // Set up orthographic projection matrix for 2D rendering.
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    // Apply projection matrix to shaders.
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // --- Load Textures ---
    // Load various textures used in the game (e.g., ball, paddle, background).
    ResourceManager::LoadTexture("../textures/teal_ball.png", true, "face");
    ResourceManager::LoadTexture("../textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../textures/text_box.png", true, "text_box");
    ResourceManager::LoadTexture("../textures/block.png", false, "block");
    ResourceManager::LoadTexture("../textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("../textures/teal-particle.png", true, "particle");

    // --- Initialize Renderers ---
    // Initialize renderers for sprites, particles, and text.
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 750);
    Text = new TextRenderer(this->Width, this->Height);

    // Load font for text rendering.
    Text->Load("../fonts/ARJULIAN.TTF", 24);

    // --- Load Levels ---
    // Load level data from files and initialize levels.
    GameLevel one; one.Load("../levels/1.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("../levels/2.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("../levels/3.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("../levels/4.lvl", this->Width, this->Height / 2);
    GameLevel five; five.Load("../levels/5.lvl", this->Width, this->Height / 2);
    GameLevel six; six.Load("../levels/6.lvl", this->Width, this->Height / 2);

    // Add levels to the levels container.
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Levels.push_back(five);
    this->Levels.push_back(six);

    // Create/Open the high score database.
    db = new HighScoreDB("highscores.db");




    // Create tables for each level (if they don't already exist).
    for (int level = 0; level < 6; ++level) {
        if (!db->createTable(level)) {
            std::cerr << "Failed to create table for level " << level << "." << std::endl;
        }
    }

    // --- Configure Game Objects ---
    // Set initial player position and ball position.
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);

    // Create player paddle and ball objects.
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

// Update the game state, handling ball movement, collisions, particle updates, 
// loss condition, win condition, and high score checks.
void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);  // Update ball position based on delta time.
    this->DoCollisions();         // Check for collisions (ball, paddle, and bricks).
    Particles->Update(dt, *Ball,1, glm::vec2(Ball->Radius / 2.0f));  // Update particles.

    // Check for loss condition (ball fell below screen).
    if (Ball->Position.y >= this->Height) // If ball reaches the bottom of the screen.
    {
        --this->Lives;  // Deduct one life.

        // If the player has no lives left, set the game state to "GAME_OVER".
        if (this->Lives == 0)
        {
            this->State = GAME_OVER;
            this->Lives = 3;   // Reset lives for the next game.
        }
        this->ResetPlayer();   // Reset player position and ball state for a new round.
    }

    // Check if the player has won.
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        StopLevelTimer();

        // Check if the player has achieved a high score for the current level.
        if (!db->isNewHighScore(this->Level, levelCompletionTime))
        {
            // Set the game state to "GAME_WIN" if the player wins, 
            // but doesn't set a high score.
            this->State = GAME_WIN;  
        }
        else {
            this->State = HIGH_SCORE;
        }
        this->Lives = 3;      // Reset lives for the next round.
        this->ResetPlayer();  // Reset player position and ball state for the next level.
    }
}


// Handle character input for entering the player's name in the high score screen.
 void Game::ProcessCharInput(char c)
{
    if (this->State == HIGH_SCORE)
    {
        // Only allow alphanumeric characters and spaces, with a max length of 10 characters for player name.
        if (isalnum(c) || c == ' ')
        {
            if (this->playerName.length() < 10)
            {
                this->playerName += c;   // Append the character to player name.
            }
        }
    }
}

// Process user input (keyboard actions) for different game states.
void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        // If ENTER is pressed, start the game.
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;  // Mark ENTER as processed.
        }

        // If W is pressed, move to the next level
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 6;     // Cycle through levels (0-5).
            this->KeysProcessed[GLFW_KEY_W] = true;  // Mark W as processed.
        }

        // If S is pressed, move to the previous level.
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0)
            {
                --this->Level;    // Move to previous level
            }
            else
            {
                this->Level = 5;  // Wrap around to the last level.
            }
            this->KeysProcessed[GLFW_KEY_S] = true;   // Mark S as processed.
        }

        // If H is pressed, show the high score display screen.
        if (this->Keys[GLFW_KEY_H])
        {
            this->KeysProcessed[GLFW_KEY_H] = true;
            this->State = HIGH_SCORE_DISPLAY;
        }
    }

    if (this->State == GAME_WIN || this->State == GAME_OVER)
    {
        // If ENTER is pressed, reset level and return to the main menu.
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->ResetLevel();       // Reset the level state.
            this->State = GAME_MENU;
        }
    }

    if (this->State == HIGH_SCORE)
    {
        // Handle backspace to remove last character from player name
        if (this->Keys[GLFW_KEY_BACKSPACE] && !this->KeysProcessed[GLFW_KEY_BACKSPACE] && !playerName.empty())
        {
            playerName.pop_back();  // Remove last character
            this->KeysProcessed[GLFW_KEY_BACKSPACE] = true;  // Mark backspace as processed.
        }

        // If ENTER is pressed, confirm player name input and add score to the database.
        if (this->Keys[GLFW_KEY_ENTER] && !this->playerName.empty())
        {

            db->addScore(this->Level, this->playerName, this->levelCompletionTime);

            // Reset name input, reset level, and proceed to high score display screen.
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->playerName.clear();
            this->ResetLevel();
            this->State = HIGH_SCORE_DISPLAY;
        }
    }

    if (this->State == HIGH_SCORE_DISPLAY)
    {
        // If ENTER is pressed, return to the main menu.
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->State = GAME_MENU;
        }
    }

    if (this->State == GAME_ACTIVE)
    {
        // Normalize velocity based on delta time.
        float velocity = PLAYER_VELOCITY * dt;

        // Move player paddle to the left if LEFT or A key is pressed.
        if (this->Keys[GLFW_KEY_LEFT] || this->Keys[GLFW_KEY_A])
        {
            Player->Velocity.x = -PLAYER_VELOCITY;  // Save player velocity for ball/paddle collisions.
            if (Player->Position.x - 1 >= 0.0f)     // Ensure paddle doesn't move off screen
            {
                Player->Position.x -= velocity;     // Move player left
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;   // Move the ball with the paddle if it is stuck
            }
        }
        // Move player paddle to the right if RIGHT or D key is pressed.
        else if (this->Keys[GLFW_KEY_RIGHT] || this->Keys[GLFW_KEY_D])
        {
            Player->Velocity.x = PLAYER_VELOCITY;  // Save player velocity for ball/paddle collisions.
            if (Player->Position.x <= this->Width - Player->Size.x - 1)  // Ensure paddle stays within screen bounds.
            {
                Player->Position.x += velocity;    // Move player right.
                if (Ball->Stuck)
                    Ball->Position.x += velocity;  // Move the ball with the paddle if it is stuck.
            }
        }
        else
        {
            Player->Velocity.x = 0.0f;   // Set player velocity to 0 if no keys are pressed.
        }

        // Spacebar releases the ball from the paddle.
        if (this->Keys[GLFW_KEY_SPACE])
        {
            if (Ball->Stuck == true)
            {
                glm::vec2 oldVelocity = Ball->Velocity;         // Save the ball's current velocity.
                Ball->Velocity.x = Player->Velocity.x * 0.9f;   // Allow paddle's velocity to affect the ball's velocity.

                // Normalize and maintain the ball's speed.
                float ballSpeed = glm::length(oldVelocity);
                Ball->Velocity = glm::normalize(Ball->Velocity) * ballSpeed;

                // Start the level timer if the player is beginning a new level.
                if (this->Lives == 3)
                {
                    StartLevelTimer();
                }

                Ball->Stuck = false;   // Release the ball from the paddle
            }
        }
    }
}

void Game::Render()
{
    // If the game is active, in the menu, or the win state, render the game elements.
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
    {
        // Draw the background texture at full screen size.
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), 
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);

        // Draw the current level.
        this->Levels[this->Level].Draw(*Renderer);  

        // Draw the player's paddle.
        Player->Draw(*Renderer);

        // Draw particle effects while ball is in motion.
        if ((Ball->Stuck && Player->Velocity.x != 0) || !Ball->Stuck)
        Particles->Draw();

        // Draw the ball.
        Ball->Draw(*Renderer);

        // If the game is active, display the player's remaining lives.
        if (this->State == GAME_ACTIVE) {
            std::stringstream ss;
            ss << this->Lives;   // Convert lives count to string.
            Text->RenderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f, glm::vec3(1.0f, 0.9f, 0.9f));
        }
    }

    // If the game is in the menu state, render menu instructions and options.
    if (this->State == GAME_MENU)
    {
        // Render menu text instructions for starting the game and navigating levels.
        Text->RenderCenteredText("Press ENTER to start", this->Height / 2.0f + 20.0f, Width, 1.0f);
        Text->RenderCenteredText("Press W or S to select level", this->Height / 2.0f + 45.0f, Width, 0.75f);
        Text->RenderCenteredText("Once the game begins, use spacebar to release the ball", this->Height / 2.0f + 85.0f, Width, 0.75, glm::vec3(1.0f, 0.8f, 0.3f));
        Text->RenderCenteredText("Move the paddle left with the 'A' key/left arrow and right with the 'D' key/right arrow", this->Height / 2.0f +105.0f, Width, 0.75, glm::vec3(1.0f, 0.8f, 0.3f));
        Text->RenderCenteredText("Clear all breakable bricks to complete each level!", this->Height / 2.0f +125.0f, Width, 0.75, glm::vec3(1.0f, 0.8f, 0.3f));
        Text->RenderCenteredText("Press the 'H' key to view the high scores for the currently selected level", this->Height / 2.0f + 175.0f, Width, 0.75f, glm::vec3(1.0f, 0.8f, 0.3f));
    }

    // If the game has been won, but a high score has not been achieved, display the win screen with the completion time.
    if (this->State == GAME_WIN)
    {
        Text->RenderCenteredText("YOU WON!", this->Height / 2.0f - 30.0f, Width, 1.5f, glm::vec3(0.1f, 0.9f, 0.2f));

        // Format the level completion time to 2 decimal places.
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << this->levelCompletionTime;
        std::string formattedTime = ss.str();

        // Render the completion time text.
        Text->RenderCenteredText("Completion time: " + formattedTime + " seconds", this->Height / 2.0f + 30.0f, Width, 1.2f, glm::vec3(0.1f, 0.9f, 0.2f));
        Text->RenderCenteredText("Improve your time to add your name to the leaderboard", this->Height / 2.0f + 60.0f, Width, 1.2f, glm::vec3(0.1f, 0.9f, 0.2f));
        Text->RenderCenteredText("Press ENTER to return to the level select screen or ESC to quit", this->Height / 2.0f + 150.0f, Width, 1.0f);
    }

    // If the high score screen is active, display the congratulatory message and obtain user input for high score.
    if (this->State == HIGH_SCORE)
    {
        // Define the dimensions and position for the high score input box.
        float boxWidth = this->Width / 1.2f; // 20% smaller than the screen width
        float boxHeight = this->Height / 1.2f; // 20% smaller than the screen height

        // Center the box in the middle of the screen.
        float boxPosX = (this->Width - boxWidth) / 2.0f; 
        float boxPosY = (this->Height - boxHeight) / 2.0f; 

        // Draw the background box for the high score entry.
        Renderer->DrawSprite(ResourceManager::GetTexture("text_box"),
            glm::vec2(boxPosX, boxPosY), glm::vec2(boxWidth, boxHeight), 0.0f);

        // Display congratulatory text and instructions to input player name.
        Text->RenderCenteredText("CONGRATULATIONS!", 90.0f, Width, 1.5f, glm::vec3(0.1f, 0.9f, 0.2f));
        Text->RenderCenteredText("Your completion time of this level has placed you on the leaderboard!", 150.0f, Width, 0.7f, glm::vec3(0.1f, 0.9f, 0.2f));
        Text->RenderCenteredText("Please enter your username below", 170.0f, Width, 0.7f, glm::vec3(0.1f, 0.9f, 0.2f));
        Text->RenderCenteredText("and it will be added to the high score list for this level", 190.0f, Width, 0.7f, glm::vec3(0.1f, 0.9f, 0.2f));

        // Display the entered player name.
        Text->RenderCenteredText(playerName, 250.0f, Width, 1.2f, glm::vec3(0.0f, 1.0f, 0.0f));

        /// Display the instruction to confirm the name entry.
        Text->RenderCenteredText("Press ENTER to confirm", this->Height / 1.8f, Width, 0.75f, glm::vec3(1.0f, 0.8f, 0.3f));
    }

    // If the high score display is active, render the high scores for the current level.
    if (this->State == HIGH_SCORE_DISPLAY)
    {
        // Display the high scores heading.
        Text->RenderCenteredText("HIGH SCORES", 70.0f, Width, 1.5f, glm::vec3(0.1f, 0.9f, 0.2f));

        // Retrieve the high scores for the current level.
        std::vector<HighScore> highScores = db->getHighScores(this->Level);

        // Define column positions for displaying high score information.
        float rankX = 220.0f;
        float nameX = 290.0f;
        float completionTimeX = 420.0f;
        float yStart = 130.0f;  
        float yOffset = 40.0f;

        // Loop through the high scores and render each entry.
        for (int i = 0; i < highScores.size(); ++i)
        {
            // Obtain high score from the vector of high scores.
            HighScore highscore = highScores[i];

            // Render rank number.
            Text->RenderText(std::to_string(i + 1), rankX, yStart + (i * yOffset), 1.0f, glm::vec3(0.9f, 0.1f, 0.1f));

            // Render player's name.
            Text->RenderText(highscore.playerName, nameX, yStart + (i * yOffset), 1.0f, glm::vec3(0.9f, 0.1f, 0.1f));

            // Format the completion time to 2 decimal places and render it.
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << highscore.completionTime;
            std::string formattedCompletionTime = stream.str();
            Text->RenderText(formattedCompletionTime + " seconds", completionTimeX, yStart + (i * yOffset), 1.0f, glm::vec3(0.9f, 0.1f, 0.1f));
        }

        // Display instructions to return to the main menu.
        Text->RenderCenteredText("Press ENTER to return to the level select screen or ESC to quit", this->Height / 2.0f + 150.0f, Width, 1.0f);
    }

    // If the game is over, display the game over message.
    if (this->State == GAME_OVER)
    {
        Text->RenderCenteredText("OH NO! You ran out of Lives!", this->Height / 2.0f + 40.0f, Width, 1.0f, glm::vec3(0.9f, 0.1f, 0.1f));
        Text->RenderCenteredText("Press ENTER to return to the level select screen or ESC to quit", this->Height / 2.0f + 80.0f, Width, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

// --- Collision Handling Helper Function Declarations ---

// Checks for AABB-AABB (Axis-Aligned Bounding Box) collision.
bool CheckCollision(GameObject& one, GameObject& two);

// Checks for AABB-Circle collision and returns collision data (collision occurred, direction, and difference vector).
Collision CheckCollision(BallObject& one, GameObject& two);

// Determines the closest cardinal direction (UP, DOWN, LEFT, RIGHT) based on a given vector.
Direction VectorDirection(glm::vec2 closest);

// Handles ball-brick collision resolution by adjusting velocity and position.
void ResolveBrickCollision(BallObject& ball, Direction dir, const glm::vec2& diff_vector);

// Handles ball-paddle collision resolution by adjusting velocity based on impact position.
void ResolvePaddleCollision(const Collision& collision);

// --- Collision Handling ---

// Handles all collision detection and resolution for the game.
void Game::DoCollisions()
{
    // Check for collisions between ball and bricks
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // If collsion occurred
            {
                // Destroy brick if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;

                // Resolve collision by adjusting ball velocity and position.
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                ResolveBrickCollision(*Ball, dir, diff_vector);
            }
        }
    }
    // Check for collisions between ball and paddle (unless stuck).
    if (!Ball->Stuck)
    {
        // Calculate whether a collision between the ball and paddle has occurred.
        Collision paddleCollision = CheckCollision(*Ball, *Player);
        if (std::get<0>(paddleCollision))
        {
            ResolvePaddleCollision(paddleCollision);
        }
    }
}

// Resets the bricks for the current level.
void Game::ResetLevel()
{
    std::string fileName = "../levels/" + std::to_string(this->Level + 1) + ".lvl";
    this->Levels[this->Level].Load(fileName, this->Width, this->Height / 2);
}

// Resets the player paddle and ball to their starting positions.
void Game::ResetPlayer() const
{
    Player->Size = PLAYER_SIZE;
    Player->Velocity.x = 0.0f;

    // Reset paddle position only at the start of a new game.
    if (this->Lives == 3)
    {
        Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    }

    // Reset ball position and velocity.
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

// Starts the level completion timer.
void Game::StartLevelTimer() {
    levelStartTime = std::chrono::high_resolution_clock::now();
}

// Stops the level completion timer and records the duration.
void Game::StopLevelTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    levelCompletionTime = std::chrono::duration<float>(endTime - levelStartTime).count();
}

// Returns the last recorded level completion time.
float Game::GetLevelCompletionTime() const {
    return levelCompletionTime;
}

// --- Helper Functions ---

// Performs AABB-AABB collision detection between two game objects.
bool CheckCollision(GameObject& one, GameObject& two)
{
    // Check collision along the x-axis.
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;

    // Check collision along the y-axis.
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;

    // Return true if both x and y axes overlap (collision detected)
    return collisionX && collisionY;
}

// Performs AABB-Circle collision detection and returns the collision data.
Collision CheckCollision(BallObject& one, GameObject& two)
{
    // Calculate the center of the ball.
    glm::vec2 center(one.Position + one.Radius);

    // Calculate AABB (Axis-Aligned Bounding Box) info: center and half-extents.
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    // Calculate the difference vector between the circle center and the AABB center.
    glm::vec2 difference = center - aabb_center;

    // Clamp the difference vector to the box's half-extents to find the closest point.
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    // Determine the closest point on the AABB.
    glm::vec2 closest = aabb_center + clamped;

    // Compute the vector between the ball center and the closest point.
    difference = closest - center;

    // Check if the distance between the circle center and the closest point is less than the radius
    if (glm::length(difference) < one.Radius) 
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// Determines the closest cardinal direction (UP, DOWN, LEFT, RIGHT) for a given vector.
Direction VectorDirection(glm::vec2 target)
{
    // Directions for compass: up, right, down, left
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f), // Up
        glm::vec2(1.0f, 0.0f), // Right
        glm::vec2(0.0f, -1.0f), //Down
        glm::vec2(-1.0f, 0.0f) // Left
    };

    float max = 0.0f;
    unsigned int best_match = -1;

    // Loop through compass directions and find the best match using dot product.
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);

        // Update max dot product and best match if the current dot product is higher.
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    // Return the best direction match
    return (Direction)best_match;
}

// Resolves ball-brick collisions by adjusting the ball's velocity and position.
void ResolveBrickCollision(BallObject& ball, Direction dir, const glm::vec2& diff_vector)
{
    // Horizontal collision
    if (dir == LEFT || dir == RIGHT)
    {
        // Reverse horizontal velocity.
        ball.Velocity.x = -ball.Velocity.x;

        // Adjust ball position to resolve penetration.
        float penetration = ball.Radius - std::abs(diff_vector.x);
        ball.Position.x += (dir == LEFT ? penetration : -penetration);
    }
    // Vertical collision
    else
    {
        // Reverse vertical velocity.
        ball.Velocity.y = -ball.Velocity.y;

        // Adjust ball position to resolve penetration.
        float penetration = ball.Radius - std::abs(diff_vector.y);
        ball.Position.y += (dir == UP ? -penetration : penetration);
    }
}

// Resolves ball-paddle collisions by adjusting velocity based on impact position.
void ResolvePaddleCollision(const Collision& collision)
{
    BallObject& ball = *Ball;
    GameObject& paddle = *Player;
    float paddleVelocityInfluence = 0;

    // Calculate where the ball struck the paddle.
    float centerBoard = paddle.Position.x + paddle.Size.x / 2.0f;
    float distance = (ball.Position.x + ball.Radius) - centerBoard;
    float percentage = distance / (paddle.Size.x / 2.0f);

    // Apply a portion of the paddle's velocity, if its direction agrees with the percentage calculation
    if ((percentage >= 0 && paddle.Velocity.x >= 0) || (percentage < 0 && paddle.Velocity.x < 0)) 
    {
        // Calculate the paddle's influence on the ball's x velocity.
        paddleVelocityInfluence = paddle.Velocity.x * 0.25f;
    }

    // Update the ball's velocity direction.
    float strength = 0.5f;
    glm::vec2 oldVelocity = ball.Velocity;
    ball.Velocity.x = (glm::length(ball.Velocity) * percentage * strength) + paddleVelocityInfluence;

    // Normalize and maintain the ball's speed.
    float ballSpeed = glm::length(oldVelocity);
    ball.Velocity = glm::normalize(ball.Velocity) * ballSpeed;

    // Adjust for sticky paddle.
    ball.Velocity.y = -std::abs(ball.Velocity.y);
}