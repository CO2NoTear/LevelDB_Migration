////
// @file statusTest.cc
// @brief
// 测试Status
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#include <catch2/catch_test_macros.hpp>
#include <leveldb/status.h>
#include <leveldb/config.h>
#include <string.h>

using namespace leveldb;

TEST_CASE("store/status.h")
{
    SECTION("ok")
    {
        Status status;
        REQUIRE(status.ok());
    }
}
