#pragma once

#include "FontFace.hpp"
#include "FontTexture.hpp"

// #include "../Graphics/TextureView.hpp"

#include <nodec/containers/sparse_table.hpp>
#include <nodec/vector2.hpp>

#include <string>
#include <unordered_map>

namespace std {
template<>
struct hash<std::pair<FT_Face, std::uint16_t>> {
    std::size_t operator()(const std::pair<FT_Face, std::uint16_t> &x) const {
        return reinterpret_cast<const std::size_t>(x.first) ^ x.second;
    }
};
} // namespace std

struct FontCharacter {
    bool initialized{false};

    //! Size of glyph (in pixels).
    nodec::Vector2i size;

    //! Offset from baseline to left/top of glyph (in pixels).
    nodec::Vector2i bearing;

    //! the horizontal advance e.g. the horizontal distance (in 1/64th pixels)
    //! from the origin to the origin of the next glyph.
    std::uint16_t advance;

    std::unique_ptr<FontTexture> pFontTexture;
};

class FontCharacterDatabase {
    struct FaceBlock {
        std::uint16_t pixelSize;
        nodec::containers::SparseTable<FontCharacter> characters;
    };

public:
    FontCharacterDatabase(Graphics *pGfx)
        : mpGfx{pGfx} {}

    const FontCharacter &Get(FT_Face face, std::uint16_t pixelSize, std::uint32_t chCode) {
        auto &faceBlock = mFaces[{face, pixelSize}];
        auto &character = faceBlock.characters[chCode];

        if (character.initialized) return character;

        FT_Set_Pixel_Sizes(face, 0, pixelSize);

        if (FT_Load_Char(face, chCode, FT_LOAD_RENDER)) {
            // TODO: return dummy character with no throw.
            throw std::runtime_error("Failed to load Glyph");
        }

        character.advance = face->glyph->advance.x;
        character.size.set(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        character.bearing.set(face->glyph->bitmap_left, face->glyph->bitmap_top);
        character.pFontTexture.reset(new FontTexture(mpGfx, face));

        character.initialized = true;

        return character;
    }

private:
    Graphics *mpGfx;
    std::unordered_map<std::pair<FT_Face, std::uint16_t>, FaceBlock> mFaces;
};