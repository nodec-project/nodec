#pragma once

#include "../Font/FontFace.hpp"

#include <string>

struct FontCharacter {
};

class FontCharacterDatabase {

public:


    const FontCharacter& GetFontCharacter(FontFace& face, int pixelSize, std::uint32_t chCode) {
        // TODO: Check cached font character. and return.

        // TODO: If not cached, make the new one.

        // TODO: Cache font character.

        // TODO: return font character.


        // FT_Load_Char()

    }

};