#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace asura {
class Logger {
public:
    static void init() {
        logger = spdlog::stdout_color_mt("logsura");
        logger->set_pattern("[%Y-%m-%d | %H:%M:%S:%e] [%l] %v");
        has_init = true;
    }
    static std::shared_ptr<spdlog::logger> log() { return logger; }
private:
    static std::shared_ptr<spdlog::logger> logger;
    static bool has_init;
};
}