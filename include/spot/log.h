#pragma once

#include <fmt/format.h>

#include <string>

namespace spot
{
void loge(const std::string& msg);

template<typename S, typename... Args>
void loge(const S& msg, Args&&... args)
{
    loge(fmt::format(msg, std::forward<Args>(args)...));
}

void logi(const std::string& msg);

template<typename S, typename... Args>
void logi(const S& msg, Args&&... args)
{
    logi(fmt::format(msg, std::forward<Args>(args)...));
}

void logs(const std::string& msg);

template<typename S, typename... Args>
void logs(const S& msg, Args&&... args)
{
    logs(fmt::format(msg, std::forward<Args>(args)...));
}

void log_up(const std::string& msg);

template<typename S, typename... Args>
void log_up(const S& msg, Args&&... args)
{
    log_up(fmt::format(msg, std::forward<Args>(args)...));
}

void log_down(const std::string& msg);

template<typename S, typename... Args>
void log_down(const S& msg, Args&&... args)
{
    log_down(fmt::format(msg, std::forward<Args>(args)...));
}

} // namespace spot
