#ifndef APP__RESOURCES_HPP_
#define APP__RESOURCES_HPP_

#include "app.hpp"

class Resources : public nodec_modules::game_engine::interfaces::Behavior
{
    using Material = nodec_modules::rendering::interfaces::Material;

private:
    static NodecObject::Reference<Resources> instance;

public:
    static Resources* get();

public:
    using Behavior::Behavior;

public:
    NodecObject::Reference<Material> material_acoustic_foam;

public:
    void on_awake() override;


};


#endif