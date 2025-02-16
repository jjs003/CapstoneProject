/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"


// GameLevel represents a Breakout game level and handles loading,
// rendering, and checking level completion based on tile destruction.
class GameLevel
{
public:
    // Public member to hold all bricks for the level.
    std::vector<GameObject> Bricks;

    // Default constructor
    GameLevel() { }

    // Loads level from a file and initializes tile data.
    void Load(std::string file, unsigned int levelWidth, unsigned int levelHeight);

    // Renders the current level's tiles (bricks)
    void Draw(SpriteRenderer& renderer);

    // Checks if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();

private:
    // Private helper function to initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif

