#pragma once

#include <algorithm>

// This macro can be used as an expression for if conditionals
#define FIND(c, e, i)                                                                              \
    auto i = std::find(std::begin(c), std::end(c), e);                                             \
    i != std::end(c)

#define FIND_IF(c, e, i)                                                                           \
    auto i = std::find_if(std::begin(c), std::end(c), e);                                          \
    i != std::end(c)

namespace spot
{

template<typename C, typename P>
auto find(C& v, const P& e)
{
    return std::find(std::begin(v), std::end(v), e);
}

template<typename C, typename P>
auto find_if(C& v, const P& e)
{
    return std::find_if(std::begin(v), std::end(v), e);
}

} // namespace spot
