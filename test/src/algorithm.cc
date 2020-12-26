#include <spot/algorithm.h>

#include <catch2/catch.hpp>

namespace spot
{
TEST_CASE("algorithm")
{
    std::vector<int> container = {1, 2, 3};

    SECTION("can use element")
    {
        int element = 2;
        auto result = find(container, element);
        REQUIRE(result != std::end(container));
    }

    SECTION("can use lambda")
    {
        auto is_two = [](const int& e) { return e == 2; };
        auto result = find_if(container, is_two);
        REQUIRE(result != std::end(container));
    }

    SECTION("can use macro")
    {
        int element = 2;
        if (FIND(container, element, result)) {
            REQUIRE(result != std::end(container));
            REQUIRE(*result == element);
        } else {
            REQUIRE(false);
        }

        auto is_two = [](const int& e) { return e == 2; };
        if (FIND_IF(container, is_two, result)) {
            REQUIRE(result != std::end(container));
            REQUIRE(*result == 2);
        } else {
            REQUIRE(false);
        }
    }
}

} // namespace spot
