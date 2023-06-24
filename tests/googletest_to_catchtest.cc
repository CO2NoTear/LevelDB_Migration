/**
 * @file googletest_to_catchtest.cc
 * @author CO2NoTear (sqede@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-06-24
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <catch2/catch_test_macros.hpp>
#define ASSERT_TRUE(exp) REQUIRE((exp) == true)

TEST_CASE("googletest syntax")
{
    SECTION("EQ")
    {
        int a = 1;
        ASSERT_TRUE(a != 2);
    }
}