/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.h"
#include "resource_manager.h"

// Constructor: Initializes the text renderer with a screen width and height.
TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
	// Load and configure the text renderer shader for 2D rendering.
	this->TextShader = ResourceManager::LoadShader("../shaders/text_2d.vs", "../shaders/text_2d.fs", nullptr, "text");
	this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
	this->TextShader.SetInteger("text", 0);

    // Configure the Vertex Array Object (VAO) and Vertex Buffer Object (VBO) for rendering texture quads.
    glGenVertexArrays(1, &this->VAO);
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindVertexArray(0);
}

// Loads a font and generates textures for each character.
void TextRenderer::Load(const std::string& font, unsigned int fontSize)
{
    // Clear previously loaded characters.
    this->Characters.clear();

    // Initialize and load FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not initialize the FreeType Library" << std::endl;
        return;
    }

    // Load the font face from the specified file.
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        FT_Done_FreeType(ft);
        return;
    }

    // Set the font size for rendering.
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // Disable byte-alignment restriction to ensure correct texture data.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load the first 128 ASCII characters and create a texture for each one.
    for (GLubyte c = 0; c < 128; ++c)
    {
        // Load character glyph.
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load glyph for character: " << c << std::endl;
            continue;
        }

        // Generate a texture for the character.
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture parameters for wrapping and filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store the character information for future rendering.
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // Clean up FreeType resources after loading the font.
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

// Renders text at a specified position, scale, and color.
void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color) 
{
    // Activate the shader program and set the text color.
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // Loop through each character in the text string.
    for (char c : text)
    {
        // Skip characters that aren't available.
        if (Characters.find(c) == Characters.end())
        {
            continue;
        }

        Character ch = Characters[c];

        // Calculate the position and size of the character quad.
        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Define the vertices for the character quad.
        float vertices[6][4] = {
            { xpos,     ypos + h,  0.0f, 1.0f },
            { xpos + w, ypos,      1.0f, 0.0f },
            { xpos,     ypos,      0.0f, 0.0f },

            { xpos,     ypos + h,  0.0f, 1.0f },
            { xpos + w, ypos + h,  1.0f, 1.0f },
            { xpos + w, ypos,      1.0f, 0.0f }
        };

        // Bind the texture for the character and update the VBO.
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Render the quad.
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance the cursor to the next character.
        x += (ch.Advance >> 6) * scale; // Convert to pixels.
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Renders text centered on the screen at the given vertical position.
void TextRenderer::RenderCenteredText(const std::string& text, float y, int width, float scale, glm::vec3 color)
{
    // Calculate the width of the text.
    float textWidth = GetTextWidth(text, scale);

    // Calculate the x position to center the text on the screen.
    float screenWidth = static_cast<float>(width);
    float xPos = (screenWidth - textWidth) / 2.0f;

    // Render the text at the calculated position.
    RenderText(text, xPos, y, scale, color);
}

// Calculates the width of a text string based on the current font and scale.
float TextRenderer::GetTextWidth(const std::string& text, float scale) {
    float width = 0.0f;

    // Loop through each character and add its width to the total.
    for (char c : text) {
        if (Characters.find(c) != Characters.end()) {
            Character ch = Characters[c];
            width += (ch.Advance >> 6) * scale;  // Convert to pixels and apply scale.
        }
    }

    return width;
}
