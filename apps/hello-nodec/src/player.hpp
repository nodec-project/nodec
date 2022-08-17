#ifndef APP__PLAYER_HPP_
#define APP__PLAYER_HPP_

#include "app.hpp"


class Player : public game_engine::Behavior
{
    using Rendering = nodec_modules::rendering::interfaces::Rendering;

public:
    using Behavior::Behavior;

    void on_awake() override;

    void on_frame_start(Rendering& rendering) override;
    void on_frame_update(Rendering& rendering) override;

};



#endif