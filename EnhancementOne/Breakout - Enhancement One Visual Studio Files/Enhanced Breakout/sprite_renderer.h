/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

// SpriteRenderer class handles rendering textured sprites
class SpriteRenderer
{
public:
    // Constructor: Initializes the shader for rendering
    SpriteRenderer(Shader& shader);

    // Destructor: Cleans up any allocated OpenGL resources
    ~SpriteRenderer();

    // Renders a quad textured with the provided sprite
    // Takes parameters for position, size, rotation, and color
    void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
    // Shader used for rendering the sprite
    Shader       shader;

    // VAO (Vertex Array Object) for the sprite's quad
    unsigned int quadVAO;

    // Initializes and configures the quad's buffer and vertex attributes for rendering
    void initRenderData();
};

#endif

