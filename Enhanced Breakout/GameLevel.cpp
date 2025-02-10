/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#include "game_level.h"

#include <fstream>
#include <sstream>


// Loads the level from the specified file, parsing tile data and initializing the game level.
void GameLevel::Load(std::string file, unsigned int levelWidth, unsigned int levelHeight)
{
    // Clear existing brick data.
    this->Bricks.clear();

    // Read the level data from the file.
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;

    if (fstream)
    {
        // Read each line from the file and parse tile data.
        while (std::getline(fstream, line)) 
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode)
            {   // Read each word separated by spaces.
                row.push_back(tileCode);
            }
            tileData.push_back(row);
        }

        // Initialize the level with the parsed tile data.
        if (tileData.size() > 0)
        {
            this->init(tileData, levelWidth, levelHeight);
        }
    }
}

// Draws all the non-destroyed bricks in the level.
void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& tile : this->Bricks)
    {
        if (!tile.Destroyed)
        {
            tile.Draw(renderer);
        }
    }
}

// Checks if the level is completed (all non-solid tiles are destroyed).
bool GameLevel::IsCompleted()
{
    for (GameObject& tile : this->Bricks)
    {
        if (!tile.IsSolid && !tile.Destroyed)
        {
            return false;
        }
    }
    return true;
}

// Initializes the level using tile data and the specified level dimensions.
void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // Calculate tile dimensions based on the level size.
    float height = static_cast<float>(tileData.size());
    float width = static_cast<float>(tileData[0].size()); // note we can index vector at [0] since this function is only called if height > 0
    float unit_width = static_cast<float>(levelWidth) / static_cast<float>(width);
    float unit_height = static_cast<float>(levelHeight) / static_cast<float>(height);

    // Initialize the level's bricks based on the tile data	.
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            // Set width, height and default color (white).
            glm::vec2 pos(unit_width * x, unit_height * y);
            glm::vec2 size(unit_width, unit_height);
            glm::vec3 color = glm::vec3(1.0f); // Default color (white).

            if (tileData[y][x] == 1) // Solid block.
            {
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            }
            else if (tileData[y][x] > 1) // Non-solid block; determine color based on tile type.
            {
                switch (tileData[y][x])
                {
                // Blue
                case 2:
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                    break;
                // Green
                case 3:
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                    break;
                // Brown
                case 4:
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                    break;
                // Orange
                case 5:
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                    break;
                default:
                    break;
                }
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
        }
    }
}

