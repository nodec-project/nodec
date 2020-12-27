#include "test_cube.hpp"

//
using namespace nodec;
using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_extentions;
using namespace nodec_extentions::material_set;

NodecObject::Reference<Mesh> TestCube::mesh_global;
NodecObject::Reference<Texture> TestCube::tex_albedo_global;
NodecObject::Reference<Texture> TestCube::tex_normal_global;
NodecObject::Reference<Texture> TestCube::tex_metallic_global;
NodecObject::Reference<Texture> TestCube::tex_roughness_global;


NodecObject::Holder<Texture> TestCube::get_texture(NodecObject::Reference<Texture> global, const std::string& path)
{
    if (auto texture = global.lock())
    {
        return texture;
    }
    auto texture = NodecObject::instanciate<Texture>(path, &(get_engine()->rendering()));
    get_engine()->rendering().bind_texture(texture.get());

    global = texture;
    return texture;
}

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
            mesh_factory::make_cube(1, *new_mesh);
            rendering.bind_mesh(new_mesh.get());
            renderer->mesh = new_mesh;
            mesh_global = new_mesh;
        }

        renderer->material = NodecObject::instanciate<BRDFMaterial>(&rendering);
        renderer->material->set_vector4("albedo", Vector4f(1, 1, 1, 1.0));
        renderer->material->set_float("metallic", 0.1f);
        renderer->material->set_float("roughness", 0.5f);

        //renderer->material->set_texture("albedo",
        //                                get_texture(tex_albedo_global, "concrete/Tcom_Pavement_PaintedConcrete3_Base_Color.tga"));
        //renderer->material->set_texture("height",
        //                                get_texture(tex_albedo_global, "concrete/Tcom_Pavement_PaintedConcrete3_Height.tga"));
        //renderer->material->set_texture("roughness",
        //                                get_texture(tex_albedo_global, "concrete/Tcom_Pavement_PaintedConcrete3_Roughness.tga"));
        //renderer->material->set_texture("normal",
        //                                get_texture(tex_albedo_global, "concrete/Tcom_Pavement_PaintedConcrete3_Normal.tga"));


        renderer->material->set_texture("albedo",
                                        get_texture(tex_albedo_global, "acoustic_foam/Tcom_Various_AcousticFoam_Base_Color.tga"));
        renderer->material->set_texture("height",
                                        get_texture(tex_albedo_global, "acoustic_foam/Tcom_Various_AcousticFoam_Height.tga"));
        renderer->material->set_texture("roughness",
                                        get_texture(tex_albedo_global, "acoustic_foam/Tcom_Various_AcousticFoam_Roughness.tga"));
        renderer->material->set_texture("metallic",
                                        get_texture(tex_albedo_global, "acoustic_foam/Tcom_Various_AcousticFoam_Metallic.tga"));
        renderer->material->set_texture("normal",
                                        get_texture(tex_albedo_global, "acoustic_foam/Tcom_Various_AcousticFoam_Normal.tga"));

        //renderer->material->set_texture("albedo",
        //                                get_texture(tex_albedo_global, "panel/Tcom_Scifi_Panel_Base_Color.tga"));
        //renderer->material->set_texture("height",
        //                                get_texture(tex_albedo_global, "panel/Tcom_Scifi_Panel_Height.tga"));
        //renderer->material->set_texture("roughness",
        //                                get_texture(tex_albedo_global, "panel/Tcom_Scifi_Panel_Roughness.tga"));
        //renderer->material->set_texture("metallic",
        //                                get_texture(tex_albedo_global, "panel/Tcom_Scifi_Panel_Metallic.tga"));
        //renderer->material->set_texture("normal",
        //                                get_texture(tex_albedo_global, "panel/Tcom_Scifi_Panel_Normal.tga"));


        //renderer->material->set_texture("albedo",
        //                                get_texture(tex_albedo_global, "rock/Tcom_Rock_CliffVolcanic_Base_Color.tga"));
        //renderer->material->set_texture("height",
        //                                get_texture(tex_albedo_global, "rock/Tcom_Rock_CliffVolcanic_Height.tga"));
        //renderer->material->set_texture("roughness",
        //                                get_texture(tex_albedo_global, "rock/Tcom_Rock_CliffVolcanic_Roughness.tga"));
        //renderer->material->set_texture("normal",
        //                                get_texture(tex_albedo_global, "rock/Tcom_Rock_CliffVolcanic_Normal.tga"));


        //renderer->material->set_texture("albedo",
        //                                get_texture(tex_albedo_global, "blanket/Tcom_Plastic_SpaceBlanketFolds_Base_Color.tga"));
        //renderer->material->set_texture("height",
        //                                get_texture(tex_albedo_global, "blanket/Tcom_Plastic_SpaceBlanketFolds_Height.tga"));
        //renderer->material->set_texture("roughness",
        //                                get_texture(tex_albedo_global, "blanket/Tcom_Plastic_SpaceBlanketFolds_Roughness.tga"));
        //renderer->material->set_texture("metallic",
        //                                get_texture(tex_albedo_global, "blanket/Tcom_Plastic_SpaceBlanketFolds_Metallic.tga"));
        //renderer->material->set_texture("normal",
        //                                get_texture(tex_albedo_global, "blanket/Tcom_Plastic_SpaceBlanketFolds_Normal.tga"));


        //renderer->material->set_texture("normal",
        //                                get_texture(tex_albedo_global, "test/NormalMap.png"));

        //renderer->material->set_texture("height",
        //                                get_texture(tex_albedo_global, "test/HeightMap.png"));

        //texture = NodecObject::instanciate<Texture>("panel/Tcom_Scifi_Panel_Base_Color.tga", &rendering);

        //texture = NodecObject::instanciate<Texture>("panel/Tcom_Scifi_Panel_Roughness.tga", &rendering);



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
