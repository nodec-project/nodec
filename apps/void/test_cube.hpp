#ifndef APP__TEST_CUBE_HPP_
#define APP__TEST_CUBE_HPP_

#include "app.hpp"

class TestCube : public nodec_modules::game_engine::interfaces::Behavior
{
    using Renderer = nodec_modules::rendering::interfaces::Renderer;
    using Rendering = nodec_modules::rendering::interfaces::Rendering;
    using Mesh = nodec_modules::rendering::interfaces::Mesh;
    using Texture = nodec_modules::rendering::interfaces::Texture;

private:
    static NodecObject::Reference<Mesh> mesh_global;
    static NodecObject::Reference<Texture> tex_albedo_global;
    static NodecObject::Reference<Texture> tex_metallic_global;
    static NodecObject::Reference<Texture> tex_roughness_global;

public:
    using Behavior::Behavior;

private:
    NodecObject::Reference<Renderer> renderer_;

public:
    void on_awake() override;

    void on_frame_start(Rendering& rendering) override;

    void on_frame_update(Rendering& rendering) override;
};



#endif