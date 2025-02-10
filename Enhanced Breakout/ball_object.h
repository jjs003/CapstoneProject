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
	// Ball properties.
	float Radius;  // The radius of the ball, which influences its collision detection.
	bool Stuck;    // Flag indicating if the ball is stuck to the paddle.

	// Default constructor: Initializes the ball with default values.
	BallObject();

	// Parameterized constructor: Initializes the ball with a specific position, radius, velocity, and texture.
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	// Moves the ball based on its velocity and the time delta, ensuring it stays within the window bounds.
	// If the ball hits the top or side, it bounces back. The updated position is returned.
	glm::vec2 Move(float dt, unsigned int window_width);

	// Resets the ball to its initial state by setting the given position and velocity.
	// This is typically used when the ball is lost, and the game needs to reset.
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif