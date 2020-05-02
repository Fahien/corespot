#pragma once

#include <functional>
#include <vector>

namespace std
{
template<typename T>
size_t hash_combine(T value)
{
    return hash<T>()(value);
}

template<typename T, typename... Targs>
size_t hash_combine(T value, Targs... args) // recursive variadic function
{
    auto hv = hash<T>()(value);
    hv ^= hash_combine(args...) + 0x9e3779b9 + (hv << 6) + (hv >> 2);
    return hv;
}

template<typename T>
struct hash<std::vector<T>> {
    size_t operator()(const std::vector<T>& vec) const
    {
        size_t hv = 0;

        for (auto& elem : vec) {
            /// @ref https://stackoverflow.com/a/2595226
            hv ^= hash<T>()(elem) + 0x9e3779b9 + (hv << 6) + (hv >> 2);
        }

        return hv;
    }
};

} // namespace std
