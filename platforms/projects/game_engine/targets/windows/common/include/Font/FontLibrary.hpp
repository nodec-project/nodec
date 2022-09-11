#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <nodec/formatter.hpp>
#include <stdexcept>

class FontLibrary {
public:
    FontLibrary() {
        if (FT_Init_FreeType(&mLibrary)) {
            throw std::runtime_error(nodec::ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                                     << "Could not initialize FreeType Library.");
        }

    }

    ~FontLibrary() {
        FT_Done_FreeType(mLibrary);
    }

private:
    FT_Library mLibrary;
};