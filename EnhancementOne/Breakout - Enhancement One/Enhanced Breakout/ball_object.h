/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"


// BallObject is a subclass of GameObject that represents the ball in Breakout.
// It contains additional properties and methods specific to the ball, such as its radius
// and the ability to move and reset the ball's state within the game.
class BallObject : public GameObject
{
public:
	// Ball properties
	float Radius;  // Radius of the ball
	bool Stuck;    // Flag indicating if the ball is stuck to the paddle

	// Constructors
	BallObject();  // Default constructor
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);  // Parameterized constructor

	// Moves the ball, ensuring it stays within the window bounds (except the bottom edge).
	// Returns the updated position of the ball.
	glm::vec2 Move(float dt, unsigned int window_width);

	// Resets the ball to its initial state, with the given position and velocity.
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif