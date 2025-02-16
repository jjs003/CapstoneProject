/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#include "sprite_renderer.h"

// Constructor that initializes the shader and sets up the necessary render data.
SpriteRenderer::SpriteRenderer(Shader& shader)
{
    this->shader = shader;  // Store the provided shader.
    this->initRenderData();  // Initialize the vertex array and buffer for rendering.
}

// Destructor that cleans up any OpenGL resources associated with the sprite renderer.
SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);   // Delete the VAO (vertex array object).
}

// Function to draw a textured sprite with transformations applied (position, rotation, scale).
void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    // Use the shader for rendering.
    this->shader.Use();

    // Create a model matrix for transforming the sprite.
    glm::mat4 model = glm::mat4(1.0f);

    // Translate the sprite to the desired position.
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order).

    // Move the origin to the center of the sprite for rotation.
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad.
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate.
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back.

    // Apply the scale to the sprite.
    model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale.

    // Pass the model matrix to the shader.
    this->shader.SetMatrix4("model", model);

    // Set the sprite color in the shader.
    this->shader.SetVector3f("spriteColor", color);

    // Bind the texture to texture unit 0 and set up the VAO for rendering.
    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    // Render the sprite using the prepared VAO.
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// Private function to initialize the vertex array and buffer for the sprite quad.
void SpriteRenderer::initRenderData()
{
    // Define the vertex data for a sprite quad (2 triangles).
    unsigned int VBO;
    float vertices[] = {
        // Vertex positions and texture coordinates.
        // pos      // tex 
        0.0f, 1.0f, 0.0f, 1.0f,   // Top-left
        1.0f, 0.0f, 1.0f, 0.0f,   // Bottom-right
        0.0f, 0.0f, 0.0f, 0.0f,   // Bottom-left

        0.0f, 1.0f, 0.0f, 1.0f,   // Top-left
        1.0f, 1.0f, 1.0f, 1.0f,   // Top-right
        1.0f, 0.0f, 1.0f, 0.0f    // Bottom-right
    };

    // Generate and configure VAO and VBO.
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    // Bind and fill the buffer with vertex data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up the vertex array object (VAO).
    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind the VAO after setup.
    glBindVertexArray(0);
}

