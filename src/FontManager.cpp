#include "FontManager.h"

namespace fs = std::filesystem;

void FontManager::Init() {
	auto error = FT_Init_FreeType(&m_library);
	if (error) {
		std::cout << "[FreeType] An error accured durning init" << std::endl;
	}
	LoadFonts();
}

void FontManager::LoadFont(std::string fontPath, RenderSettings* settings) {
	auto error = FT_New_Face(m_library, fontPath.c_str(), 0, &m_face);
	if (error == FT_Err_Unknown_File_Format) {
		std::cout << "[FreeType] Unknown file format" << std::endl;
	} else if (error) {
		std::cout << "[FreeType] An error accured durning loading font" << std::endl;
	}

	FT_Set_Pixel_Sizes(m_face, settings->characterSize[0], settings->characterSize[1]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
			std::cout << "[FreeType] Failed to load Glyph" << std::endl;
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			m_face->glyph->bitmap.width,
			m_face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
				texture,
				glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
				glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
				m_face->glyph->advance.x
		};
		m_characters.insert(std::pair<char, Character>(c, character));
	}

	std::cout << "[FreeType] Font glyph loaded." << std::endl;
	// std::cout << "[Font] " << m_face->family_name << std::endl;
}

void FontManager::LoadFonts() {
	for (const auto& entry : fs::directory_iterator(m_winFontPath)) {
		auto path = entry.path().string();
		auto sub = Substring(path, 3);
		if (sub == "ttf") {
			m_availableFonts.push_back(path);
		}
	}
}

void FontManager::Destroy() {
	FT_Done_Face(m_face);
	FT_Done_FreeType(m_library);
}

std::string FontManager::Substring(std::string input, int n) {
	return input.substr(input.size() - n);
}