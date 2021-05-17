#ifndef GAME_EDITOR__GAME_EDITOR_HPP_
#define GAME_EDITOR__GAME_EDITOR_HPP_

#include <imelements/window.hpp>


namespace game_editor {

class GameEditor {
public:
    GameEditor() = default;

public:
    virtual imelements::WindowManager& window_manager() = 0;

};




}

#endif