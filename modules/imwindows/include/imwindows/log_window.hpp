#ifndef IMWINDOWS__IMPL__LOG_WINDOW_HPP_
#define IMWINDOWS__IMPL__LOG_WINDOW_HPP_

#include <imessentials/window.hpp>

#include <nodec/formatter.hpp>
#include <nodec/logging.hpp>
#include <nodec/signals/scoped_block.hpp>
#include <nodec/signals/signal.hpp>
#include <nodec/vector2.hpp>

#include <imgui.h>

#include <mutex>
#include <queue>
#include <vector>

namespace imwindows {

class LogWindow final : public imessentials::BaseWindow {
    const int MAX_RECORDS = 100;

    struct RecordEntry {
        RecordEntry(std::uint32_t id, const nodec::logging::LogRecord &record)
            : id(id), record(record),
              formatted_text(nodec::Formatter() << record) {}

        nodec::logging::LogRecord record;
        std::string formatted_text;
        std::uint32_t id;
    };

public:
    LogWindow()
        : BaseWindow("Log", nodec::Vector2f(500, 400)), auto_scroll_(true){};

    void on_gui() override {
        using namespace nodec;
        using namespace nodec::logging;

        {
            std::lock_guard<std::mutex> lock(pending_records_mutex_);

            for (const auto &record : pending_records_) {
                record_entries_.push_back({next_id_++, record});
            }
            pending_records_.clear();
        }

        while (MAX_RECORDS < record_entries_.size()) {
            record_entries_.pop_front();
        }

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
        ImGui::BeginChild("Entries", ImVec2(0, -100), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        if (clear) record_entries_.clear();

        auto draw_record_entry = [&](RecordEntry &entry) {
            ImVec4 color;
            switch (entry.record.level) {
            case Level::Fatal:
            case Level::Error:
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                break;
            case Level::Warn:
                color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                break;
            default:
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);

            if (ImGui::Selectable(static_cast<std::string>(Formatter() << entry.formatted_text << "##" << entry.id).c_str(),
                                  entry.id == selected_id_, ImGuiSelectableFlags_None, ImVec2(0, 30))) {
                selected_id_ = entry.id;
                selected_entry_text_ = entry.formatted_text;
            }
            // ImGui::Selectable(text.c_str(), &selected, ImGuiSelectableFlags_None, ImVec2(0, 50));
            // ImGui::Text(static_cast<std::string>(Formatter() << record.level).c_str());
            // ImGui::Text(text.c_str());

            ImGui::PopStyleColor();
            // ImGui::EndChild();
        };

        if (filter_.IsActive()) {
            for (auto &entry : record_entries_) {
                if (!filter_.PassFilter(entry.formatted_text.c_str())) {
                    continue;
                }

                draw_record_entry(entry);
            }
        } else {
            ImGuiListClipper clipper;
            clipper.Begin(record_entries_.size());
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    draw_record_entry(record_entries_[i]);
                }
            }
            clipper.End();
        }

        if (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::Separator();

        ImGui::BeginChild("Details", ImVec2(0, 0), false);
        ImGui::TextWrapped(selected_entry_text_.c_str());
        ImGui::EndChild();
    }

private:
    nodec::signals::Connection logging_handler_conn_ = nodec::logging::record_handlers().connect(
        [&](const nodec::logging::LogRecord &record) {
            // This section may be called in another thread.
            using namespace nodec::signals;

            ScopedBlock<Connection> scoped_block{logging_handler_conn_};

            {
                std::lock_guard<std::mutex> lock(pending_records_mutex_);
                pending_records_.push_back(record);
            }
        });

    std::vector<nodec::logging::LogRecord> pending_records_;
    std::mutex pending_records_mutex_;
    std::deque<RecordEntry> record_entries_;
    std::uint32_t next_id_{0};
    std::uint32_t selected_id_{0xFFFFFFFF};
    std::string selected_entry_text_;
    bool auto_scroll_;
    ImGuiTextFilter filter_;
};

} // namespace imwindows

#endif