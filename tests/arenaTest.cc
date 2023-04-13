////
// @file arenaTest.cc
// @brief
// 测试Arena
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#include <catch2/catch.hpp>
#include <store/arena.h>
#include <store/random.h>

using namespace store;

TEST_CASE("store/arena.h")
{
    SECTION("Empty") { Arena arena; }

    SECTION("Simple")
    {
        std::vector<std::pair<size_t, char *> > allocated;
        Arena arena;
        const int N = 100000;
        size_t bytes = 0;
        Random rnd(301);
        for (int i = 0; i < N; i++) {
            size_t s;
            if (i % (N / 10) == 0) {
                s = i;
            } else {
                s = rnd.oneIn(4000)
                        ? rnd.uniform(6000)
                        : (rnd.oneIn(10) ? rnd.uniform(100) : rnd.uniform(20));
            }
            if (s == 0) {
                // Our arena disallows size 0 allocations.
                s = 1;
            }
            char *r;
            if (rnd.oneIn(10)) {
                r = arena.aligned(s);
            } else {
                r = arena.allocate(s);
            }

            for (size_t b = 0; b < s; b++) {
                // Fill the "i"th allocation with a known bit pattern
                r[b] = i % 256;
            }
            bytes += s;
            allocated.push_back(std::make_pair(s, r));
            REQUIRE(arena.usage() >= bytes);
            if (i > N / 10) { REQUIRE(arena.usage() <= bytes * 1.10); }
        }
        for (size_t i = 0; i < allocated.size(); i++) {
            size_t num_bytes = allocated[i].first;
            const char *p = allocated[i].second;
            for (size_t b = 0; b < num_bytes; b++) {
                // Check the "i"th allocation for the known bit pattern
                REQUIRE((int(p[b]) & 0xff) == (i % 256));
            }
        }
    }
}