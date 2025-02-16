/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This file contains the implementation of the ParticleGenerator class,
** responsible for managing and rendering particle effects. Functions include
** particle spawning, updating, and rendering, as well as initialization logic.
******************************************************************/

#include "particle_generator.h"

// ---Constants---
const float RANDOM_POSITION_SCALE = 10.0f; // Scale for randomizing particle position
const float RANDOM_COLOR_OFFSET = 0.5f;    // Base value for randomizing particle color


// Constructor: Initializes the particle generator with a shader, texture, and particle count.
ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

// Updates the state of all particles and spawns new ones.
// Parameters:
// - dt: Delta time (time elapsed since the last frame).
// - object: The GameObject influencing the particle's position and velocity (the player's ball).
// - newParticles: Number of new particles to spawn this frame.
// - offset: offset from the center of the object (edge of the ball)
void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
	// Add new particles.
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, offset);

	}

	// Update all particles.
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle& p = this->particles[i];

		// Reduce particle's life.
		p.Life -= dt;

		// If particle is alive (Life > 0.0f), update its position and fade its color over time.
		if (p.Life > 0.0f)
		{
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 3.0f;
		}
	}
}

// Renders all active particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give a "glow" effect.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.Use();

	// Set shader offset, shader color, bind the texture for each particle, then render each particle.
	for (const Particle& particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			// Set particle properties in the shader.
			this->shader.SetVector2f("offset", particle.Position);
			this->shader.SetVector4f("color", particle.Color);

			// Bind the particle texture and draw.
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}

	// Reset to default blending mode.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Initializes the particle system's OpenGL buffers and creates the particle array (used in the constructor).
void ParticleGenerator::init()
{
	// Vertex data for each particle quad
	unsigned int VBO;
	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	// Generate OpenGL buffers and configure attributes.
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);

	// Fill mesh buffer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	// Set mesh attributes.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// Create pre-defined amount (this->amount) of particle instances.
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		this->particles.push_back(Particle());
	}
}

// Variable for keeping track of the last dead particle respawned.
unsigned int lastUsedParticle = 0;

// Finds the first inactive particle or the one with the shortest life.
// Returns: The index of the particle.
unsigned int ParticleGenerator::firstUnusedParticle()
{
	// Begin search from last used particle, which should return almost instantly.
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i)
	{
		if (this->particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// If previous method is unsuccessful, use linear search.
	// Search for first particle with life <= 0, or particle with lowest life.
	unsigned int oldestParticle = 0;
	for (unsigned int i = 0; i < lastUsedParticle; ++i) 
	{
		if (this->particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
		else {
			if (this->particles[i].Life < this->particles[oldestParticle].Life) {
				oldestParticle = i;
			}
		}
	}

	// If no inactive particle is found, overwrite the oldest one.
	lastUsedParticle = oldestParticle;
	return oldestParticle;
}

// Resets the properties of a given particle.
// Parameters:
// - particle: The particle to reset.
// - object: The GameObject whose position and velocity influence the particle (the ball, in this case)
// - offset: offset from the center of the object (edge of the ball)
void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	float random = ((rand() % 100) - 50) / RANDOM_POSITION_SCALE;
	float randomColor = RANDOM_COLOR_OFFSET + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(randomColor, randomColor, randomColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}