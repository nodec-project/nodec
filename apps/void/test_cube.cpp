#include "test_cube.hpp"

//
using namespace nodec;
//using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_extentions;
using namespace nodec_extentions::material_set;

NodecObject::Reference<Mesh> TestCube::mesh_global;
NodecObject::Reference<Texture> TestCube::tex_albedo_global;
NodecObject::Reference<Texture> TestCube::tex_metallic_global;
NodecObject::Reference<Texture> TestCube::tex_roughness_global;


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

        if (auto texture = tex_albedo_global.lock())
        {
            renderer->material->set_texture("albedo", texture);
        }
        else
        {
            texture = NodecObject::instanciate<Texture>("concrete/Tcom_Pavement_PaintedConcrete3_Base_Color.tga", &rendering);
            //texture = NodecObject::instanciate<Texture>("panel/Tcom_Scifi_Panel_Base_Color.tga", &rendering);
            rendering.bind_texture(texture.get());
            renderer->material->set_texture("albedo", texture);
            tex_albedo_global = texture;
        }

        if (auto texture = tex_roughness_global.lock())
        {
            renderer->material->set_texture("roughness", texture);
        }
        else
        {
            texture = NodecObject::instanciate<Texture>("concrete/Tcom_Pavement_PaintedConcrete3_Roughness.tga", &rendering);
            //texture = NodecObject::instanciate<Texture>("panel/Tcom_Scifi_Panel_Roughness.tga", &rendering);
            rendering.bind_texture(texture.get());
            renderer->material->set_texture("roughness", texture);
            tex_roughness_global = texture;
        }

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
