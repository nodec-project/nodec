#include "InspectorGUI.hpp"


using namespace nodec_scene::components;
using namespace nodec_rendering::components;
using namespace nodec;

void InspectorGUI::on_gui_transform(Transform &trfm) {
    //static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
    //static ImGuizmo::MODE currentGizmoMode = ImGuizmo::LOCAL;

    {
        ImGui::Text("Position");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat3("##Position", trfm.local_position.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
            trfm.dirty = true;
        }
    }

    {
        static Vector3f eulerAngles;
        static bool isActive = false;

        if (!isActive) {
            eulerAngles = math::gfx::euler_angles_xyz(trfm.local_rotation);
        }

        ImGui::Text("Rotation (XYZ Euler)");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if (ImGui::DragFloat3("##Rotation", eulerAngles.v, 0.1f, -FLT_MAX, +FLT_MAX, "%.3f")) {
            trfm.local_rotation = math::gfx::euler_angles_xyz(eulerAngles);
            trfm.dirty = true;
        }

        isActive = ImGui::IsItemActive();
    }

    {
        ImGui::Text("Scale");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat3("##Scale", trfm.local_scale.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
            trfm.dirty = true;
        }
    }

    //if (currentGizmoOperation != ImGuizmo::SCALE) {
    //    if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL)) {
    //        currentGizmoMode = ImGuizmo::LOCAL;
    //    }
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD)) {
    //        currentGizmoMode = ImGuizmo::WORLD;
    //    }
    //}

    //ImGuiIO &io = ImGui::GetIO();
    //ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ////ImGuizmo::Manipulate()
}

void InspectorGUI::on_gui_camera(Camera &camera) {
    ImGui::DragFloat("Near Clip Plane", &camera.near_clip_plane);
    ImGui::DragFloat("Far Clip Plane", &camera.far_clip_plane);

    {
        int current = static_cast<int>(camera.projection);
        ImGui::Combo("Projection", &current, "Perspective\0Orthographic");
        camera.projection = static_cast<Camera::Projection>(current);
    }

    switch (camera.projection) {
    case Camera::Projection::Perspective:
        ImGui::DragFloat("Fov Angle", &camera.fov_angle);
        break;
    case Camera::Projection::Orthographic:
        ImGui::DragFloat("Ortho Width", &camera.ortho_width);
    default: break;
    }
}
