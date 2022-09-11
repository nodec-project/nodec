#pragma once

#include "FontLibrary.hpp"

class FontFace {

public:
    FontFace(FT_Library library, const std::string& path) {
        if (FT_New_Face(library, path.c_str(), 0, &mFace)) {
            throw std::runtime_error(nodec::ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                                     << "Failed to load font. path: " << path);
        }
        
    }

    ~FontFace() {
        FT_Done_Face(mFace);
    }

    FT_Face& GetFace() {
        return mFace;
    }

private:
    FT_Face mFace;
};