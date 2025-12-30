//
// Created by Shreejit Murthy on 13/12/2025
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <memory>

#define LOGSURA_INFO(...)   Asura::Log::get().info(__VA_ARGS__)
#define LOGSURA_WARN(...)   Asura::Log::get().warn(__VA_ARGS__)
#define LOGSURA_ERROR(...)  Asura::Log::get().error(__VA_ARGS__)
#define LOGSURA_CRIT(...)   Asura::Log::get().critical(__VA_ARGS__)

static constexpr int LOGSURA_KEPT_MESSAGES = 8;

namespace Asura {
class Log {
public:
    static spdlog::logger& get() {
        static auto logger = [] {
            auto rb = ringbuffer_sink();

            auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();

            std::vector<spdlog::sink_ptr> sinks { console_sink, rb };
            auto l = std::make_shared<spdlog::logger>("logsura", sinks.begin(), sinks.end());

            // l->set_pattern("\033[+%2oms]\033[0m [%^%l%$] %v");
            l->set_pattern("[+%oms %^%l%$] %v");
            l->set_level(spdlog::level::trace);
            l->flush_on(spdlog::level::info);

            spdlog::register_logger(l);

            return l;
        }();

        return *logger;
    }

    static std::vector<std::string> last_messages() {
        return ringbuffer_sink()->last_formatted();
    }

    static std::vector<std::string> last_raw_messages() {
        std::vector<std::string> out;
        auto raw = ringbuffer_sink()->last_raw();

        out.reserve(raw.size());
        for (auto &msg : raw) {
            out.emplace_back(msg.payload.begin(), msg.payload.end());
        }
        return out;
    }


    static void flush_messages() {
        ringbuffer_sink()->flush();
    }

private:
    static std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> ringbuffer_sink() {
        static auto sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(LOGSURA_KEPT_MESSAGES);
        return sink;
    }
};
} // Asura
