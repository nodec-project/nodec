#include "test_cube.hpp"

//
using namespace nodec;
//using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_extentions;
using namespace nodec_extentions::material_set;

NodecObject::Reference<Mesh> TestCube::mesh_global;


void TestCube::on_awake()
{
    enable_frame_update();
}

void TestCube::on_frame_start(Rendering& rendering)
{
    renderer_ = scene_object().add_component<Renderer>();
    if (auto renderer = renderer_.lock())
    {
        if (auto mesh = mesh_global.lock())
        {
            renderer->mesh = mesh;
        }
        else
        {
            auto new_mesh = NodecObject::instanciate<Mesh>(&rendering);
            mesh_factory::make_cube(2, *new_mesh);
            rendering.bind_mesh(new_mesh.get());
            renderer->mesh = new_mesh;
            mesh_global = new_mesh;
        }

        renderer->material = NodecObject::instanciate<BRDFMaterial>(&rendering);
        renderer->material->set_vector4("albedo", Vector4f(0, 1, 0, 1));
        renderer->material->set_float("metallic", 0.5f);
        renderer->material->set_float("roughness", 0.5f);
        rendering.bind_material(renderer->material.get());

        rendering.regist_renderer(renderer);
    }
}

void TestCube::on_frame_update(Rendering& rendering)
{
    //if (auto renderer = renderer_.lock())
    //{
    //    renderer->material->set_vector4("albedo", Vector4f(0, 1, 0, 1));
    //}
}
