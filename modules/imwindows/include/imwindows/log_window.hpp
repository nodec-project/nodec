#ifndef IMWINDOWS__IMPL__LOG_WINDOW_HPP_
#define IMWINDOWS__IMPL__LOG_WINDOW_HPP_

#include <imessentials/window.hpp>

#include <nodec/logging.hpp>
#include <nodec/signals/scoped_block.hpp>
#include <nodec/signals/signal.hpp>
#include <nodec/vector2.hpp>

#include <imgui.h>

#include <queue>

namespace imwindows {

class LogWindow final : public imessentials::BaseWindow {
    const int MAX_RECORDS = 100;

public:
    LogWindow()
        : BaseWindow("Log", nodec::Vector2f(500, 400)), auto_scroll(true){};

    void on_gui() override {
        // Options menu
        if (ImGui::BeginPopup("Options")) {
            ImGui::Checkbox("Auto-scroll", &auto_scroll);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options")) {
            ImGui::OpenPopup("Options");
        }
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");

        ImGui::SameLine();
        filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear) records.clear();

        if (filter.IsActive()) {
            for (const auto &record : records) {
                const std::string text = nodec::Formatter() << record;
                if (filter.PassFilter(text.c_str())) {
                    ImGui::TextUnformatted(text.c_str());
                }
            }
        } else {
            ImGuiListClipper clipper;
            clipper.Begin(records.size());
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    std::string text = nodec::Formatter() << records[i];
                    ImGui::TextUnformatted(text.c_str());
                }
            }
            clipper.End();
        }

        if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }

private:
    void recordHandler(const nodec::logging::LogRecord &record) {
        using namespace nodec::signals;

        //using ScopedBlock = nodec::signals::ScopedBlock<nodec::logging::RecordHandlers::Connection>;
        ScopedBlock<Connection> scoped_block{logging_hander_conn};

        records.push_back(record);
        if (records.size() > MAX_RECORDS) {
            records.pop_front();
        }
    }

private:
    nodec::signals::Connection logging_hander_conn = nodec::logging::record_handlers().connect(
        [&](const nodec::logging::LogRecord &record) {
            recordHandler(record);
        });

    std::deque<nodec::logging::LogRecord> records;
    bool auto_scroll;
    ImGuiTextFilter filter;
};

//
// inline static bool registered = game_editor::menu::register_menu_item("Window/Log", LogWindow::init);

} // namespace imwindows

#endif