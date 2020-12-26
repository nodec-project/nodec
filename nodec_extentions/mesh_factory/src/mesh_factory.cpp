#include <nodec_extentions/mesh_factory/mesh_factory.hpp>

#include <nodec/math/math.hpp>
//#include <nodec/logging.hpp>

using namespace nodec;
using namespace nodec_modules::rendering::interfaces;

namespace nodec_extentions
{
namespace mesh_factory
{
/**
* @detail
*   b   c             c   b
*   +---+  ^        ^ +---+
*   | / |  |        | | \ |
*   +---+  +-->  <--+ +---+
*   a   d             d   a
*/
static void add_quad(int a, int b, int c, int d, Mesh& mesh)
{
    mesh.triangles.emplace_back(a);
    mesh.triangles.emplace_back(b);
    mesh.triangles.emplace_back(c);
    mesh.triangles.emplace_back(a);
    mesh.triangles.emplace_back(c);
    mesh.triangles.emplace_back(d);
}

namespace cube_layout
{
static const Vector3f origins[6] = {
    Vector3f(-0.5f, -0.5f, -0.5f),
    Vector3f(+0.5f, -0.5f, -0.5f),
    Vector3f(+0.5f, -0.5f, +0.5f),
    Vector3f(-0.5f, -0.5f, +0.5f),
    Vector3f(-0.5f, +0.5f, -0.5f),
    Vector3f(-0.5f, -0.5f, +0.5f)
};
static const Vector3f normals[6] = {
    Vector3f(+0.0f, +0.0f, -1.0f),
    Vector3f(+1.0f, +0.0f, +0.0f),
    Vector3f(+0.0f, +0.0f, +1.0f),
    Vector3f(-1.0f, +0.0f, +0.0f),
    Vector3f(+0.0f, +1.0f, +0.0f),
    Vector3f(+0.0f, -1.0f, +0.0f)
};
static const Vector3f rights[6] = {
    Vector3f(+1.0f, +0.0f, +0.0f),
    Vector3f(+0.0f, +0.0f, +1.0f),
    Vector3f(-1.0f, +0.0f, +0.0f),
    Vector3f(+0.0f, +0.0f, -1.0f),
    Vector3f(+1.0f, +0.0f, +0.0f),
    Vector3f(+1.0f, +0.0f, +0.0f)
};
static const Vector3f ups[6] = {
    Vector3f(0.0f, 1.0f, 0.0f),
    Vector3f(0.0f, 1.0f, 0.0f),
    Vector3f(0.0f, 1.0f, 0.0f),
    Vector3f(0.0f, 1.0f, 0.0f),
    Vector3f(0.0f, 0.0f, 1.0f),
    Vector3f(0.0f, 0.0f, -1.0f)
};
} // namespace cube2space

void calc_tangent_space(nodec_modules::rendering::interfaces::Mesh& mesh)
{
    // <https://learnopengl.com/Advanced-Lighting/Normal-Mapping>

    std::vector<uint16_t> counts(mesh.vertices.size());
    for (auto& idx : mesh.triangles)
    {
        counts[idx]++;
    }

    for (auto iter = mesh.triangles.begin(); iter != mesh.triangles.end(); ++iter)
    {
        uint16_t idx1 = *iter++;
        uint16_t idx2 = *iter++;
        uint16_t idx3 = *iter;

        auto edge1 = mesh.vertices[idx2].position - mesh.vertices[idx1].position;
        auto edge2 = mesh.vertices[idx3].position - mesh.vertices[idx1].position;
        auto delta_uv1 = mesh.vertices[idx2].uv - mesh.vertices[idx1].uv;
        auto delta_uv2 = mesh.vertices[idx3].uv - mesh.vertices[idx1].uv;

        float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);
        nodec::Vector3f tangent(
            f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x),
            f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y),
            f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z)
        );
        //nodec::Vector3f tangent = f * (edge1 * delta_uv2.y - edge2 * delta_uv1.y);

        //nodec::Vector3f bitangent(
        //    f * (-delta_uv2.x * edge1.x + delta_uv1.x * edge2.x),
        //    f * (-delta_uv2.x * edge1.y + delta_uv1.x * edge2.y),
        //    f * (-delta_uv2.x * edge1.z + delta_uv1.x * edge2.z)
        //);

        mesh.vertices[idx1].tangent += tangent / static_cast<float>(counts[idx1]);
        //mesh.vertices[idx1].bitangent = bitangent / static_cast<float>(counts[idx1]);

        mesh.vertices[idx2].tangent += tangent / static_cast<float>(counts[idx2]);
        //mesh.vertices[idx2].bitangent = bitangent / static_cast<float>(counts[idx2]);

        mesh.vertices[idx3].tangent += tangent / static_cast<float>(counts[idx3]);
        //mesh.vertices[idx3].bitangent = bitangent / static_cast<float>(counts[idx3]);
    }
}

void make_cube(int divisions, Mesh& mesh)
{
    const float step = 1.0f / static_cast<float>(divisions);
    const Vector3f step3(step, step, step);

    for (auto face = 0; face < 6; ++face)
    {
        const Vector3f origin = cube_layout::origins[face];
        const Vector3f right = cube_layout::rights[face];
        const Vector3f up = cube_layout::ups[face];
        const Vector3f normal = cube_layout::normals[face];

        // up
        for (auto j = 0; j < divisions + 1; ++j)
        {
            const Vector3f j3(j, j, j);
            // right
            for (auto i = 0; i < divisions + 1; ++i)
            {
                const Vector3f i3(i, i, i);
                Vector3f p = origin + step3 * (i3 * right + j3 * up);
                Vector2f uv(step * i, 1.0f - step * j);
                //logging::DebugStream(__FILE__, __LINE__) << p;
                
                mesh.vertices.emplace_back(p, normal, uv);
            }
        }
    }

    const int k = divisions + 1;
    for (auto face = 0; face < 6; ++face)
    {
        for (auto j = 0; j < divisions; ++j)
        {
            //const bool bottom = j < (divisions / 2);
            for (auto i = 0; i < divisions; ++i)
            {
                //const bool left = i < (divisions / 2);
                const int a = (face * k + j) * k + i;
                const int b = (face * k + j) * k  + i + 1;
                const int c = (face * k + j + 1) * k + i;
                const int d = (face * k + j + 1) * k + i + 1;

                // c   d             d   c
                // +---+  ^        ^ +---+
                // |   |  |        | |   |
                // +---+  +-->  <--+ +---+
                // a   b             b   a
                add_quad(a, c, d, b, mesh);
            }
        }
    }

    calc_tangent_space(mesh);
}

void make_normalized_cube(int divisions, Mesh& mesh)
{
    
}
//
//void make_spherified_cube(int divisions, Mesh& mesh)
//{
//    const float step = 1.0f / float(divisions);
//    const Vector3f step3(step, step, step);
//
//    for (auto face = 0; face < 6; ++face)
//    {
//
//    }
//}


}
}