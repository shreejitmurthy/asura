#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace Asura {
inline spdlog::logger& log() {
    static auto logger = []{
        auto l = spdlog::stdout_color_mt("logsura");
        l->set_pattern("\033[2m[%T.%e | +%oms]\033[0m [%^%l%$] %v");
        return l;
    }();
    return *logger;
}
} // Asura
