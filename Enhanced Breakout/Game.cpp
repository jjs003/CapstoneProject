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


#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include <iostream>

// --- Global Variables ---
// Game-related render objects
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;

// --- Game Class Implementation ---

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Level(0)
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
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
    ResourceManager::LoadTexture("../textures/hot_ball2.png", true, "face");
    ResourceManager::LoadTexture("../textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../textures/block.png", false, "block");
    ResourceManager::LoadTexture("../textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("../textures/red-particle-triangle2.png", true, "particle");

    // --- Initialize Renderers ---
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 750);

    // --- Load Levels ---
    GameLevel one; one.Load("../levels/1.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("../levels/2.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("../levels/3.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("../levels/4.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);

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
        this->ResetLevel();
        this->ResetPlayer();
    }
}

const void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // Move player paddle
        if (this->Keys[GLFW_KEY_LEFT] || this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x - 1 >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_RIGHT] || this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x - 1)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;  // Release the ball from the paddle
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), 
                             glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        this->Levels[this->Level].Draw(*Renderer);  // Draw current level
        Player->Draw(*Renderer);                    // Draw player paddle
        Particles->Draw();                          // Draw particle effects
        Ball->Draw(*Renderer);                      // Draw the ball
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
}

// Resets the player and ball
void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
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

    // Calculate new ball velocity after collision
    float strength = 2.0f;
    glm::vec2 oldVelocity = ball.Velocity;
    ball.Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    ball.Velocity = glm::normalize(ball.Velocity) * glm::length(oldVelocity);

    // Adjust for sticky paddle
    ball.Velocity.y = -std::abs(ball.Velocity.y);
}

