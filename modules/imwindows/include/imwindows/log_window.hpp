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
        : BaseWindow("Log", nodec::Vector2f(500, 400)), auto_scroll_(true){};

    void on_gui() override {
        // Options menu
        if (ImGui::BeginPopup("Options")) {
            ImGui::Checkbox("Auto-scroll", &auto_scroll_);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options")) {
            ImGui::OpenPopup("Options");
        }
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");

        ImGui::SameLine();
        filter_.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear) records_.clear();

        if (filter_.IsActive()) {
            for (const auto &record : records_) {
                const std::string text = nodec::Formatter() << record;
                if (filter_.PassFilter(text.c_str())) {
                    ImGui::TextUnformatted(text.c_str());
                }
            }
        } else {
            ImGuiListClipper clipper;
            clipper.Begin(records_.size());
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    std::string text = nodec::Formatter() << records_[i];
                    ImGui::TextUnformatted(text.c_str());
                }
            }
            clipper.End();
        }

        if (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }

private:
    void recordHandler(const nodec::logging::LogRecord &record) {
        using namespace nodec::signals;

        // using ScopedBlock = nodec::signals::ScopedBlock<nodec::logging::RecordHandlers::Connection>;
        ScopedBlock<Connection> scoped_block{logging_handler_conn_};

        records_.push_back(record);
        if (records_.size() > MAX_RECORDS) {
            records_.pop_front();
        }
    }

private:
    nodec::signals::Connection logging_handler_conn_ = nodec::logging::record_handlers().connect(
        [&](const nodec::logging::LogRecord &record) {
            recordHandler(record);
        });

    std::deque<nodec::logging::LogRecord> records_;
    bool auto_scroll_;
    ImGuiTextFilter filter_;
};

} // namespace imwindows

#endif