#include <spot/handle.h>

#include <catch2/catch.hpp>
#include <string>

struct Thing : public spot::Handled<Thing> {
    Thing(std::string n)
        : name {std::move(n)}
    {
    }

    std::string name;
};

TEST_CASE("handle")
{
    Thing::Handle chair;
    REQUIRE(!chair);

    chair = Thing::create("chair");
    REQUIRE(chair);
    REQUIRE(chair == chair->handle);
    REQUIRE(chair.get_index() == 0);
    // REQUIRE(chair.get_generation() == 0);
    REQUIRE(chair->name == "chair");

    auto desk = Thing::create("desk");
    REQUIRE(desk);
    REQUIRE(desk.get_index() == 1);
    REQUIRE(desk.get_generation() == 0);
    REQUIRE(desk->name == "desk");

    chair->handle.invalidate();
    REQUIRE(!chair);

    auto monitor = Thing::create("monitor");
    REQUIRE(monitor);
    REQUIRE(monitor.get_index() == chair.get_index());
    REQUIRE(monitor.get_generation() == chair.get_generation() + 1);
    REQUIRE(monitor != chair);
}
