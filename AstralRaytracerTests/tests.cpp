#include "UtilsTests/TextureData_Test.h"
#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
#ifdef NDEBUG
    return 0;
#endif

    ::testing::InitGoogleTest(&argc, argv);
    // Turn off logging for tests
    spdlog::default_logger_raw()->set_level(spdlog::level::level_enum::off);
    return RUN_ALL_TESTS();
}