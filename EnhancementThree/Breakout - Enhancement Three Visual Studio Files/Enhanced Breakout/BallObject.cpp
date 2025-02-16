/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#include "ball_object.h"

// Default constructor initializes with a default radius and "stuck" state.
BallObject::BallObject()
	:GameObject(), Radius(12.5f), Stuck(true) { }

// Constructor that allows for custom position, radius, velocity, and sprite.
BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
	:GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true) { }

// Moves the ball, checking window bounds and updating position/velocity.
glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
	// If the ball is not stuck to the player paddle.
	if (!this->Stuck)
	{
		// Update position based on velocity.
		this->Position += this->Velocity * dt;

		// Check for collisions with window boundaries and adjust velocity/position.
		if (this->Position.x <= 0.0f)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = 0.0f;  // Prevent moving out of bounds.
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = window_width - this->Size.x;  // Prevent moving out of bounds.
		}
		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;  // Prevent moving out of bounds.
		}
	}
	return this->Position;
}


// Resets the ball to a new position and velocity while setting "stuck" state to true.
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this-> Velocity = velocity;
	this->Stuck = true;  // Ball is stuck initially after reset.
}
