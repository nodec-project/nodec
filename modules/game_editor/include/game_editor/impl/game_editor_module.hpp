#ifndef GAME_EDITOR__IMPL__GAME_EDITOR_MODULE_HPP_
#define GAME_EDITOR__IMPL__GAME_EDITOR_MODULE_HPP_

#include <game_editor/game_editor.hpp>
#include <imelements/impl/window_impl.hpp>
#include <imelements/impl/menu_impl.hpp>
//#include <scene_set/scene_registry.hpp>
#include <game_engine/impl/game_engine_module.hpp>

#include <memory>


namespace game_editor {
namespace impl {


class GameEditorModule : public GameEditor {
public:
    GameEditorModule();

public:
    void play();
    void pause();
    void step();
    void reset();

    imelements::WindowManager& window_manager() {
        return window_manager_impl_;
    }

    imelements::impl::WindowManagerImpl& window_manager_impl() {
        return window_manager_impl_;
    }

    game_engine::GameEngine& engine() {
        return *engine_module_;
    }


    game_engine::impl::GameEngineModule& engine_module() {
        return *engine_module_;
    }


private:
    //nodec::signals::Signal<void(scene_set::SceneRegistry*)> scene_registry_changed_;

    std::unique_ptr<game_engine::impl::GameEngineModule> engine_module_;
    //scene_set::SceneRegistry scene_registry;
    imelements::impl::WindowManagerImpl window_manager_impl_;


};

}
}

#endif