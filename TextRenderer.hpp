#pragma once

/*
 * All text rendering code based on the following sources:
 * https://freetype.org/freetype2/docs/tutorial/step1.html
 * https://freetype.org/freetype2/docs/tutorial/example1.c
 * https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
 * https://learnopengl.com/In-Practice/Text-Rendering
 * https://github.com/15-466/15-466-f22-base1/blob/main/PPU466.cpp
 * https://github.com/15-466/15-466-f22-base4
 */

#include <ft2build.h>
#include FT_FREETYPE_H 
#include <hb.h>
#include <hb-ft.h>

#include <glm/glm.hpp>
#include <map>

class TextRenderer {

public:
    TextRenderer(std::string font_file, uint8_t font_size) = default;
    ~TextRenderer();
    void draw(std::string text, float x, float y, float scale, glm::vec3 color, float window_width, float window_height);

private:
    FT_Face ft_face;
    FT_Library ft_library;

    hb_font_t* hb_font;
    hb_buffer_t* hb_buffer;
    hb_glyph_info_t* info;
    hb_glyph_position_t* pos;

    // From: https://learnopengl.com/In-Practice/Text-Rendering
    struct Character {
        GLuint TextureID;        // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };
    std::map<uint8_t, Character> Characters;

    // Geometry for rendering text
    GLuint VAO;
    GLuint VBO;

    // compiled with gl_compile_program on initialization
    GLuint text_shaders;
    
    // Text that this renderer is currently prepared to draw
    std::string cur_text;
};