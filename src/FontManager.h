#pragma once

#include <memory>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "glm.hpp"
#include "GLAD/glad.h"

#include "RenderSettings.h"

struct Character {
	unsigned int textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class FontManager {
public:
	void Init();
	void LoadFont(std::string fontPath, RenderSettings* settings);
	void Destroy();

	std::vector<std::string> GetAvailableFonts() { return m_availableFonts; }
	std::map<char, Character> GetCharacters() { return m_characters;  }

private:
	void LoadFonts();
	std::string Substring(std::string input, int n);

	FT_Library m_library = NULL;
	FT_Face m_face = NULL;

	std::vector<std::string> m_availableFonts;
	std::map<char, Character> m_characters;
	std::string m_winFontPath = "C:/Windows/Fonts";
};