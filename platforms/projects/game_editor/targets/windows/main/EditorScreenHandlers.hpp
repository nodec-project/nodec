#pragma once

#include <ScreenHandlers.hpp>

class EditorScreenHandlers : public ScreenHandlers {
    using Base = ScreenHandlers;

public:
    void HandleTitleChangeOnRuntime(
        screen::impl::ScreenModule& screenModule,
        const std::string& title
    ) override {
        Base::HandleTitleChangeOnRuntime(screenModule, title + " - Editor Mode");
        screenModule.title_internal = title;
    }
};

