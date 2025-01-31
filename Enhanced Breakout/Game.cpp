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

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include <iostream>
#include "text_renderer.h"

// --- Global Variables ---
// Game-related render objects
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
TextRenderer* Text;

// --- Game Class Implementation ---

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0), Lives(3)
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Text;
}

void Game::Init()
{
    // --- Load Shaders ---
    ResourceManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("../shaders/particle.vs", "../shaders/particle.fs", nullptr, "particle");

    // --- Configure shaders ---
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // --- Load Textures ---
    ResourceManager::LoadTexture("../textures/teal_ball.png", true, "face");
    ResourceManager::LoadTexture("../textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../textures/block.png", false, "block");
    ResourceManager::LoadTexture("../textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("../textures/teal-particle.png", true, "particle");

    // --- Initialize Renderers ---
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 750);
    Text = new TextRenderer(this->Width, this->Height);

    // Load font
    Text->Load("../fonts/ARJULIAN.TTF", 24);

    // --- Load Levels ---
    GameLevel one; one.Load("../levels/1.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("../levels/2.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("../levels/3.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("../levels/4.lvl", this->Width, this->Height / 2);
    GameLevel five; five.Load("../levels/5.lvl", this->Width, this->Height / 2);
    GameLevel six; six.Load("../levels/6.lvl", this->Width, this->Height / 2);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Levels.push_back(five);
    this->Levels.push_back(six);


    // --- Configure Game Objects ---
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);  // Update ball position
    this->DoCollisions();         // Check for collisions
    Particles->Update(dt, *Ball,1, glm::vec2(Ball->Radius / 2.0f));  // Update particles

    // Check loss condition (ball fell below screen)
    if (Ball->Position.y >= this->Height) // did ball reach the bottom edge?
    {
        // Player loses a life
        --this->Lives;

        // Has the player lost (Lives = 0)?
        if (this->Lives == 0)
        {
            this->State = GAME_OVER;
        }
        this->ResetPlayer();
    }

    // Check if the player has won
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->ResetPlayer();
        this->State = GAME_WIN;
    }
}

const void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 6;
            this->KeysProcessed[GLFW_KEY_W] = true;
        }
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0)
            {
                --this->Level;
            }
            else
            {
                this->Level = 5;
            }
            this->KeysProcessed[GLFW_KEY_S] = true;
        }
    }

    if (this->State == GAME_WIN || this->State == GAME_OVER)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->ResetLevel();
            this->State = GAME_MENU;
        }
    }

    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;

        // Move player paddle
        if (this->Keys[GLFW_KEY_LEFT] || this->Keys[GLFW_KEY_A])
        {
            Player->Velocity.x = -PLAYER_VELOCITY;
            if (Player->Position.x - 1 >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        else if (this->Keys[GLFW_KEY_RIGHT] || this->Keys[GLFW_KEY_D])
        {
            Player->Velocity.x = PLAYER_VELOCITY;
            if (Player->Position.x <= this->Width - Player->Size.x - 1)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        else
        {
            Player->Velocity.x = 0.0f;
        }

        if (this->Keys[GLFW_KEY_SPACE])
        {
            if (Ball->Stuck == true)
            {
                // Add a portion of the paddle's x velocity to the ball's x velocity
                glm::vec2 oldVelocity = Ball->Velocity;
                Ball->Velocity.x = Player->Velocity.x * 0.5f;

                // Normalize and maintain the ball's speed.
                float ballSpeed = glm::length(oldVelocity);
                Ball->Velocity = glm::normalize(Ball->Velocity) * ballSpeed;

                // Release the ball from the paddle
                Ball->Stuck = false;
            }
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
    {
        // Draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), 
                             glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);

        // Draw current level
        this->Levels[this->Level].Draw(*Renderer);  

        // Draw player paddle
        Player->Draw(*Renderer);

        // Draw particle effects while ball is in motion
        if ((Ball->Stuck && Player->Velocity.x != 0) || !Ball->Stuck)
        Particles->Draw();

        // Draw the ball
        Ball->Draw(*Renderer);

        // Render text
        std::stringstream ss;
        ss << this->Lives;
        Text->RenderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f, glm::vec3(1.0f, 0.9f, 0.9f));
    }

    if (this->State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", 280.0f, this->Height / 2.0f + 20.0f, 1.0f);
        Text->RenderText("Press W or S to select level", 280.0f, this->Height / 2.0f + 45.0f, 0.75f);
        Text->RenderText("Once the game begins, use spacebar to release the ball", 170.0f, this->Height / 2.0f + 85.0f, 0.75, glm::vec3(1.0f, 0.8f, 0.3f));
        Text->RenderText("Move the paddle left with the 'A' key/left arrow and right with the 'D' key/right arrow", 20.0f, this->Height / 2.0f +105.0f, 0.75, glm::vec3(1.0f, 0.8f, 0.3f));
        Text->RenderText("Clear all breakable bricks to complete each level!", 190.0f, this->Height / 2.0f +125.0f, 0.75, glm::vec3(1.0f, 0.8f, 0.3f));
    }

    if (this->State == GAME_WIN)
    {
        Text->RenderText("YOU WON!", 310.0f, this->Height / 2.0f + 25.0f, 1.5f, glm::vec3(0.1f, 0.9f, 0.2f));
        Text->RenderText("Press ENTER to return to the level select screen or ESC to quit", 35.0f, this->Height / 2.0f + 75.0f, 1.0f);
    }

    if (this->State == GAME_OVER)
    {
        Text->RenderText("OH NO! You ran out of Lives!", 235.0f, this->Height / 2.0f + 40.0f, 1.0f, glm::vec3(0.9f, 0.1f, 0.1f));
        Text->RenderText("Press ENTER to return to the level select screen or ESC to quit", 30.0f, this->Height / 2.0f + 80.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

// --- Collision Handling Helper Function Declarations ---

bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);
void ResolveBrickCollision(BallObject& ball, Direction dir, const glm::vec2& diff_vector);
void ResolvePaddleCollision(const Collision& collision);

// --- Collision Handling ---
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
                // Destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;

                // Collision resolution
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

// Resets the bricks in the level
void Game::ResetLevel()
{
    std::string fileName = "../levels/" + std::to_string(this->Level + 1) + ".lvl";
    this->Levels[this->Level].Load(fileName, this->Width, this->Height / 2);

    this->Lives = 3;
}

// Resets the player and ball
void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Velocity.x = 0.0f;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

// --- Helper Functions ---

// AABB - AABB collision detection
bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // Check collision on x-axis
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;

    // Check collision on y-axis
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;

    // Return true if both x and y axes collide
    return collisionX && collisionY;
}

// --- Collision Check: AABB - Circle Collision ---
Collision CheckCollision(BallObject& one, GameObject& two)
{
    // Calculate the center of the circle (ball)
    glm::vec2 center(one.Position + one.Radius);

    // Calculate AABB (Axis-Aligned Bounding Box) info: center and half-extents
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    // Calculate difference vector between the circle center and the AABB center
    glm::vec2 difference = center - aabb_center;

    // Clamp the difference vector to the box's half-extents
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    // Add the clamped value to the AABB center to get the closest point on the box
    glm::vec2 closest = aabb_center + clamped;

    // Calculate the vector between the circle center and the closest point on the AABB
    difference = closest - center;

    // Check if the distance between the circle center and the closest point is less than the radius
    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// --- Direction Calculation: Determines the direction of a vector (N, S, E, W) ---
Direction VectorDirection(glm::vec2 target)
{
    // Directions for compass: up, right, down, left
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f), // up
        glm::vec2(1.0f, 0.0f), // right
        glm::vec2(0.0f, -1.0f), //down
        glm::vec2(-1.0f, 0.0f) // left
    };

    // Initialize the max dot product for comparison
    float max = 0.0f;
    unsigned int best_match = -1;

    // Loop through compass directions and find the best match using dot product
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);

        // Update max dot product and best match if the current dot product is higher
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    // Return the best direction match
    return (Direction)best_match;
}

void ResolveBrickCollision(BallObject& ball, Direction dir, const glm::vec2& diff_vector)
{
    // Horizontal collision
    if (dir == LEFT || dir == RIGHT)
    {
        // Reverse horizontal velocity.
        ball.Velocity.x = -ball.Velocity.x;

        // Calculate the ball's pentration into the brick
        float penetration = ball.Radius - std::abs(diff_vector.x);

        // Correct ball's position.
        ball.Position.x += (dir == LEFT ? penetration : -penetration);
    }
    // Vertical collision
    else
    {
        // Reverse vertical velocity.
        ball.Velocity.y = -ball.Velocity.y;

        // Calculate the ball's penetration into the brick.
        float penetration = ball.Radius - std::abs(diff_vector.y);

        // Correct the ball's position.
        ball.Position.y += (dir == UP ? -penetration : penetration);
    }
}

void ResolvePaddleCollision(const Collision& collision)
{
    BallObject& ball = *Ball;
    GameObject& paddle = *Player;

    // Calculate where ball struck paddle.
    float centerBoard = paddle.Position.x + paddle.Size.x / 2.0f;
    float distance = (ball.Position.x + ball.Radius) - centerBoard;
    float percentage = distance / (paddle.Size.x / 2.0f);

    // Calculate the paddle's influence on the ball's x velocity.
    float paddleInfluence = paddle.Velocity.x * 0.25f;

    // Calculate the new x velocity
    float strength = 0.5f;
    glm::vec2 oldVelocity = ball.Velocity;
    ball.Velocity.x = (glm::length(ball.Velocity) * percentage * strength) + paddleInfluence;

    // Normalize and maintain the ball's speed.
    float ballSpeed = glm::length(oldVelocity);
    ball.Velocity = glm::normalize(ball.Velocity) * ballSpeed;

    // Adjust for sticky paddle.
    ball.Velocity.y = -std::abs(ball.Velocity.y);
}

