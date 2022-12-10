#pragma once

#include <Graphics/Graphics.hpp>
#include <Rendering/SceneRenderer.hpp>

#include <imessentials/window.hpp>
#include <nodec_rendering/components/camera.hpp>
#include <nodec_scene/scene.hpp>

#include <nodec/matrix4x4.hpp>
#include <nodec/math/gfx.hpp>

#include <ImGuizmo.h>
#include <imgui.h>

class SceneViewWindow : public imessentials::BaseWindow {
    // TODO
    const UINT WIDTH = 640;
    const UINT HEIGHT = 480;

public:
    static void init(imessentials::WindowManager &manager,
                     Graphics &gfx, nodec_scene::Scene &scene, SceneRenderer &renderer) {
        auto &window = manager.get_window<SceneViewWindow>();
        window.setup(gfx, scene, renderer);
        ImGui::SetWindowFocus(window.name());
    }

    void setup(Graphics &gfx, nodec_scene::Scene &scene, SceneRenderer &renderer) {
        // Generate the render target textures.
        D3D11_TEXTURE2D_DESC texture_desc{};
        texture_desc.Width = WIDTH;
        texture_desc.Height = HEIGHT;
        texture_desc.MipLevels = 1;
        texture_desc.ArraySize = 1;
        texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        texture_desc.SampleDesc.Count = 1;
        texture_desc.Usage = D3D11_USAGE_DEFAULT;
        texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        ThrowIfFailedGfx(
            gfx.GetDevice().CreateTexture2D(&texture_desc, nullptr, &texture_),
            &gfx, __FILE__, __LINE__);

        // Generate the render target views.
        {
            D3D11_RENDER_TARGET_VIEW_DESC desc{};
            desc.Format = texture_desc.Format;
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

            ThrowIfFailedGfx(
                gfx.GetDevice().CreateRenderTargetView(texture_.Get(), &desc, &render_target_view_),
                &gfx, __FILE__, __LINE__);
        }

        // Generate the shader resource views.
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.Format = texture_desc.Format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = 1;

            ThrowIfFailedGfx(
                gfx.GetDevice().CreateShaderResourceView(texture_.Get(), &desc, &shader_resource_view_),
                &gfx, __FILE__, __LINE__);
        }

        scene_ = &scene;
        renderer_ = &renderer;
    }

public:
    SceneViewWindow()
        : BaseWindow("Scene View##EditorWindows", nodec::Vector2f(640, 480)) {
    }

    void on_gui() override {
        using namespace nodec;

        // ImGui::BeginChildFrame(100, ImVec2(WIDTH, HEIGHT));
        ImGui::BeginChild("SceneRender", ImVec2(WIDTH, HEIGHT), false, ImGuiWindowFlags_NoMove);

        const float window_width = (float)ImGui::GetWindowWidth();
        const float window_height = (float)ImGui::GetWindowHeight();

        const float view_manipulate_right = ImGui::GetWindowPos().x + window_width;
        const float view_manipulate_top = ImGui::GetWindowPos().y;

        auto &io = ImGui::GetIO();

        {
            auto forward = math::inv(view_) * Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
            forward = math::normalize(forward);
            forward *= io.MouseWheel;
            view_.c[3] += forward;
        }

        //logging::InfoStream(__FILE__, __LINE__) << io.MouseWheel;

        ImGuizmo::SetDrawlist();

        renderer_->Render(*scene_, view_, projection_, render_target_view_.Get(), WIDTH, HEIGHT);

        ImGui::Image((void *)shader_resource_view_.Get(), ImVec2(WIDTH, HEIGHT));

        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, WIDTH, HEIGHT);

        ImGuizmo::DrawGrid(view_.m, projection_.m, Matrix4x4f::identity.m, 100.f);
        ImGuizmo::ViewManipulate(view_.m, 0.8f, ImVec2(view_manipulate_right - 128, view_manipulate_top), ImVec2(128, 128), 0x10101010);

        ImGui::EndChild();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_;

    nodec::Matrix4x4f projection_;
    nodec::Matrix4x4f view_;
    nodec_scene::Scene *scene_;
    SceneRenderer *renderer_;
};