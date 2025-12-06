#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace Asura {
static spdlog::logger& log() {
    static auto logger = []{
        auto l = spdlog::stdout_color_mt("logsura");
        l->set_pattern("[%Y-%m-%d | %T.%e] [%^%l%$] %v");
        return l;
    }();
    return *logger;
}
}
