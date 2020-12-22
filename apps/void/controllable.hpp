#ifndef APP__CONTROLLABLE_HPP_
#define APP__CONTROLLABLE_HPP_

#include "app.hpp"


class Controllable : public nodec_modules::game_engine::interfaces::Behavior
{
    using Rendering = nodec_modules::rendering::interfaces::Rendering;

public:
    using Behavior::Behavior;

    std::vector<nodec::scene_set::SceneObject*> child_ptrs;

    void on_awake() override;

    void on_frame_update(Rendering& rendering) override;

};



#endif