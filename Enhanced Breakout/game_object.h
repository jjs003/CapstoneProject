/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"


// GameObject serves as a container for all relevant state
// needed for a single entity in the game. Each object in
// the game (such as a ball, paddle, or brick) will need
// a similar state description provided by this class.
class GameObject
{
public:
    // Object state variables
    glm::vec2   Position;  // Position of the object in 2D space.
    glm::vec2   Size;      // Size of the object (width and height).
    glm::vec2   Velocity;  // Velocity of the object (movement per frame).
    glm::vec3   Color;     // RGB color of the object (default is white).
    float       Rotation;  // Rotation angle of the object (in degrees).
    bool        IsSolid;   // Whether the object is can be destroyed or not.
    bool        Destroyed; // Whether the object has been destroyed (used for game logic).

    // Texture (sprite) used to render the object.
    Texture2D   Sprite;

    // Default constructor initializes the object with default values.
    GameObject();

    // Parameterized constructor initializes the object with specific values.
    // Parameters:
    // - pos: Position of the object.
    // - size: Size of the object.
    // - sprite: Texture (sprite) for rendering the object.
    // - color: RGB color for the object (default is white).
    // - velocity: Velocity of the object (default is (0,0)).
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    // Draw the sprite on the screen using the provided renderer
    // Parameters:
    // - renderer: The SpriteRenderer object used to draw the sprite.
    virtual void Draw(SpriteRenderer& renderer);
};

#endif

