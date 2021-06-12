#ifndef GAME_EDITOR__GAME_EDITOR_HPP_
#define GAME_EDITOR__GAME_EDITOR_HPP_

#include "selection.hpp"

#include <imelements/window.hpp>
#include <game_engine/game_engine.hpp>


namespace game_editor {

class GameEditor {
public:
    GameEditor() = default;

public:
    virtual void reset() = 0;
    virtual imelements::WindowManager& window_manager() = 0;
    virtual game_engine::GameEngine& engine() = 0;
    //virtual Selection& selection() = 0;

};




}

#endif