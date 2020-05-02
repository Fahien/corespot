#include <spot/handle.h>

#include <catch2/catch.hpp>
#include <string>

namespace spot
{
struct Thing : public Handled<Thing> {
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
        REQUIRE(chair == chair->handle);
        REQUIRE(chair.get_index() == 0);
        REQUIRE(chair.get_generation() == 0);
        REQUIRE(chair->name == "chair");

        auto desk = things.push("desk");
        REQUIRE(desk);
        REQUIRE(desk.get_index() == 1);
        REQUIRE(desk.get_generation() == 0);
        REQUIRE(desk->name == "desk");
        REQUIRE(desk == things.find(1));

        auto hash = std::hash<Handle<Thing>>();
        REQUIRE(hash(chair) != hash(desk));

        chair->handle.invalidate();
        REQUIRE(!chair);

        auto monitor = things.push("monitor");
        REQUIRE(monitor);
        REQUIRE(monitor.get_index() == chair.get_index());
        REQUIRE(monitor.get_generation() == chair.get_generation() + 1);
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