////
// @file statusTest.cc
// @brief
// 测试Status
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#include <catch2/catch.hpp>
#include <store/status.h>

using namespace store;

TEST_CASE("store/status.h")
{
    SECTION("ok")
    {
        Status status;
        REQUIRE(status.get() == S_OK);

        status.set(-1);
        const char *ret = status.descript();
        REQUIRE(::strcmp(ret, "Operation not permitted") == 0);
        REQUIRE(status.isSystemError());
        status.set(-2);
        ret = status.descript();
        REQUIRE(::strcmp(ret, "No such file or directory") == 0);
        REQUIRE(status.isSystemError());

        status.set(-1000);
        ret = status.descript();
        REQUIRE(::strcmp(ret, "local error") == 0);
        REQUIRE(!status.isSystemError());

        status.set(0);
        REQUIRE(!status.isSystemError());
        status.set(33);
        REQUIRE(!status.isSystemError());
    }
}
