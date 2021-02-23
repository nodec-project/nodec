#ifndef GAME_ENGINE__BEHAVIOR_HPP_
#define GAME_ENGINE__BEHAVIOR_HPP_

#include <nodec_modules/rendering/interfaces/rendering.hpp>
#include <nodec/scene_set/component.hpp>

namespace game_engine
{

class Behavior : public nodec::scene_set::Component
{
public:
    using Component::Component;

public:
    virtual void on_frame_start(
        nodec_modules::rendering::interfaces::Rendering& rendering
    );

    virtual void on_frame_update(
        nodec_modules::rendering::interfaces::Rendering& rendering
    );

protected:
    void enable_frame_update();

private:
    bool is_called_frame_start = false;
    void on_frame_update_(nodec_modules::rendering::interfaces::Rendering& rendering);

    nodec::event::MemberCallback<Behavior, nodec_modules::rendering::interfaces::Rendering&>::SharedPtr
        on_frame_update_callback;
};

}

#endif