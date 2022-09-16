#pragma once

#include "FontFace.hpp"

#include <nodec_rendering/resources/font.hpp>

#include <string>

class FontBackend : public nodec_rendering::resources::Font {

public:
    FontBackend(FontLibrary &library, const std::string& path)
        : mFace{library.GetLibrary(), path} {
        

    }

    FT_Face GetFace() {
        return mFace.GetFace();
    }

private:
    FontFace mFace;
};

