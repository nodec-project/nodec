#ifndef NODEC_MODULES__GAME_ENGINE__INTERFACES__BEHAVIOR_HPP_
#define NODEC_MODULES__GAME_ENGINE__INTERFACES__BEHAVIOR_HPP_

#include <nodec_modules/rendering/interfaces/rendering.hpp>
#include <nodec/scene_set/component.hpp>

namespace nodec_modules
{
namespace game_engine
{
namespace interfaces
{

class Behavior : public nodec::scene_set::Component
{
public:
    using Component::Component;

public:
    virtual
        void
        on_frame_update(nodec_modules::rendering::interfaces::Rendering& rendering);

protected:
    void enable_frame_update();

private:
    void on_frame_update_(nodec_modules::rendering::interfaces::Rendering& rendering);

    nodec::event::MemberCallback<Behavior, nodec_modules::rendering::interfaces::Rendering&>::SharedPtr
        on_frame_update_callback;
};

} //  namespace interfaces
}
}
#endif