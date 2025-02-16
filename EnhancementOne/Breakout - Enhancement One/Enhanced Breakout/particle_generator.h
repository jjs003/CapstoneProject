/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** ParticleGenerator manages and renders particle effects used in
** the game.
******************************************************************/


#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "game_object.h"

// Represents a particle and its current state.
struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float     Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

// ParticleGenerator allows a large number of particles to be spawned,
// updated, and rendered. It uses a Shader for rendering and a Texture2D
// to define the appearance of particles. 
class ParticleGenerator
{
public:
	// Constructor.
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

	// Update all particles.
	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	// Render all particles.
	void Draw();

private:
	// State.
	std::vector<Particle> particles;
	unsigned int amount;

	// Render state.
	Shader shader;
	Texture2D texture;
	unsigned int VAO;

	// Initializes the buffer and vertex attributes required for rendering particles.
	void init();

	// Returns the first particle index that's currently unused (Life <= 0) or 0 if there are no currently active particles.
	unsigned int firstUnusedParticle();

	// Resets the properties of a particle (e.g., position, color, Life) to renew it.
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif