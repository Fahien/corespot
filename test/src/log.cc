#include <spot/log.h>

#include <catch2/catch.hpp>

namespace spot
{
TEST_CASE("log")
{
    logi("This is info\n");
    logi("This is info {}\n", "formatted");

    logs("This is success\n");
    logs("This is success {}\n", "formatted");

    loge("This is fail\n");
    loge("This is fail {}\n", "formatted");

    log_up("Up\n");
    log_down("Down\n");
}

} // namespace spot
