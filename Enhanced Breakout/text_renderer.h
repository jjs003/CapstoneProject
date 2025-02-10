/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	unsigned int TextureID = 0;  // ID handle of the glyph texture
	glm::ivec2   Size;           // Size of the glyph
	glm::ivec2   Bearing;        // Offset from baseline to the left/top of the glyph
	unsigned int Advance = 0;    // The horizontal offset to advance to the next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A sinle font is loaded, processed into a list of character
// items for later rendering.
class TextRenderer
{
public:
	// Holds a list of pre-compiled characters
	std::map<char, Character> Characters;

	// Shader used for text rendering
	Shader TextShader;

	// Constructor
	TextRenderer(unsigned int width, unsigned int height);

	// Pre-compiles a list of characters from the given font
	void Load(const std::string& font, unsigned int fontSize);

	// Renders a string of text using the pre-compiled list of characters
	void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

	// Determines the text width for a string for use in centering text
	float GetTextWidth(const std::string& text, float scale);

	// Creates a horizontally centered rendering for text
	void RenderCenteredText(const std::string& text, float y, int width, float scale, glm::vec3 color = glm::vec3(1.0f));


private:
	// Render state
	unsigned int VAO, VBO;
};


#endif