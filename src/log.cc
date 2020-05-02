#include "spot/log.h"

#include <fmt/color.h>
#include <fmt/format.h>

namespace spot
{
void loge(const std::string& msg)
{
    static std::string fail = fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::red), "FAIL");
    fmt::print("[{}] {}", fail, msg);
}

void logi(const std::string& msg)
{
    static std::string info =
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::deep_sky_blue), "INFO");
    fmt::print("[{}] {}", info, msg);
}

void logs(const std::string& msg)
{
    static std::string ok =
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::forest_green), " OK ");
    fmt::print("[{}] {}", ok, msg);
}

void log_up(const std::string& msg)
{
    static std::string up = fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::white), "↑");
    static std::string down =
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::dark_slate_gray), "↓");
    fmt::print("[ {}{} ] {}", up, down, msg);
}

void log_down(const std::string& msg)
{
    static std::string up =
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::dark_slate_gray), "↑");
    static std::string down = fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::white), "↓");
    fmt::print("[ {}{} ] {}", up, down, msg);
}

} // namespace spot
