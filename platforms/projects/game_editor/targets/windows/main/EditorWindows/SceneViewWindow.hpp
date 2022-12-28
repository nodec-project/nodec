#pragma once

#include <Graphics/Graphics.hpp>
#include <Rendering/SceneRenderer.hpp>

#include <imessentials/window.hpp>
#include <nodec_rendering/components/camera.hpp>
#include <nodec_scene/scene.hpp>

#include <nodec/math/gfx.hpp>
#include <nodec/matrix4x4.hpp>

//#include <ImGuizmo.h>
#include <imgui.h>

class SceneViewWindow : public imessentials::BaseWindow {
    // TODO: Support resizing window.
    const UINT VIEW_WIDTH = 640;
    const UINT VIEW_HEIGHT = 480;

public:
    SceneViewWindow(Graphics &gfx, nodec_scene::Scene &scene, SceneRenderer &renderer);

    void on_gui() override;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_;

    nodec::Matrix4x4f projection_;
    nodec::Matrix4x4f view_;
    nodec_scene::Scene *scene_{nullptr};
    SceneRenderer *renderer_{nullptr};
};