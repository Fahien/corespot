#include <spot/handle.h>

#include <catch2/catch.hpp>
#include <string>

namespace spot
{
struct Thing {
    Thing(std::string n)
        : name {std::move(n)}
    {
    }

    bool operator==(const Thing& o) const
    {
        return name == o.name;
    }

    std::string name;
};

TEST_CASE("handle")
{
    Handle<Thing> desk;
    REQUIRE(!desk);

    SECTION("section")
    {
        Handle<Thing> chair;
        REQUIRE(!chair);

        auto& things = Pack<Thing>::get();

        chair = things.push("chair");
        REQUIRE(chair);
        REQUIRE(chair.get_id() == 0);
        REQUIRE(chair.get_generation() == 0);
        REQUIRE(chair->name == "chair");

        auto desk = things.push("desk");
        REQUIRE(desk);
        REQUIRE(desk.get_id() == 1);
        REQUIRE(desk.get_generation() == 0);
        REQUIRE(desk->name == "desk");
        REQUIRE(desk == things.find(1));

        auto hash = std::hash<Handle<Thing>>();
        REQUIRE(hash(chair) != hash(desk));

        SECTION("invalidating a handle invalidates all handles to the same element")
        {
            things.find(0).invalidate();
            REQUIRE(!chair);
        }

        auto monitor = things.push("monitor");
        REQUIRE(monitor);
        REQUIRE(monitor.get_id() == 0);
        REQUIRE(monitor.get_generation() == 1);
        REQUIRE(monitor.get_id() == chair.get_id());
        REQUIRE(monitor != chair);

        auto clone_monitor = monitor.clone();
        REQUIRE(clone_monitor);
        REQUIRE(clone_monitor != monitor);
        REQUIRE(&*clone_monitor != &*monitor);
        REQUIRE(*clone_monitor == *monitor);
    }

    REQUIRE(!desk);
}

} // namespace spot