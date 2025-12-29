//
// Created by Shreejit Murthy on 13/12/2025
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

#define LOGSURA_INFO(...)   Asura::Log::get().info(__VA_ARGS__)
#define LOGSURA_WARN(...)   Asura::Log::get().warn(__VA_ARGS__)
#define LOGSURA_ERROR(...)  Asura::Log::get().error(__VA_ARGS__)
#define LOGSURA_CRIT(...)   Asura::Log::get().critical(__VA_ARGS__)

namespace Asura {
class Log {
public:
    static spdlog::logger& get() {
        static auto logger = []{
            auto l = spdlog::stderr_color_mt("logsura");
            l->set_pattern("\033[2m[%T.%e | +%2oms]\033[0m [%^%l%$] %v");
            return l;
        }();
        return *logger;
    }
};
} // Asura
