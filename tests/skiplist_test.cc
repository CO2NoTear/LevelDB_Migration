// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
/**
 * @file skiplist_test.cc
 * @author CO2NoTear (sqede@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-06-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <atomic>
#include <set>
#include <catch2/catch_test_macros.hpp>
#include "db/skiplist.h"
#include "util/arena.h"
#include "util/random.h"

namespace leveldb {

typedef uint64_t Key;

struct Comparator
{
    int operator()(const Key &a, const Key &b) const
    {
        if (a < b) {
            return -1;
        } else if (a > b) {
            return +1;
        } else {
            return 0;
        }
    }
};

TEST_CASE("skiplist test")
{
    SECTION("Empty")
    {
        Arena arena;
        Comparator cmp;
        SkipList<Key, Comparator> list(cmp, &arena);
        REQUIRE(!list.Contains(10));

        SkipList<Key, Comparator>::Iterator iter(&list);
        REQUIRE(!iter.Valid());
        iter.SeekToFirst();
        REQUIRE(!iter.Valid());
        iter.Seek(100);
        REQUIRE(!iter.Valid());
        iter.SeekToLast();
        REQUIRE(!iter.Valid());
    }

    SECTION("InsertAndLookup")
    {
        const int N = 2000;
        const int R = 5000;
        Random rnd(1000);
        std::set<Key> keys;
        Arena arena;
        Comparator cmp;
        SkipList<Key, Comparator> list(cmp, &arena);
        for (int i = 0; i < N; i++) {
            Key key = rnd.Next() % R;
            if (keys.insert(key).second) { list.Insert(key); }
        }

        for (int i = 0; i < R; i++) {
            if (list.Contains(i)) {
                REQUIRE(keys.count(i) == 1);
            } else {
                REQUIRE(keys.count(i) == 0);
            }
        }

        // Simple iterator tests
        {
            SkipList<Key, Comparator>::Iterator iter(&list);
            REQUIRE(!iter.Valid());

            iter.Seek(0);
            REQUIRE(iter.Valid());
            REQUIRE(*(keys.begin()) == iter.key());

            iter.SeekToFirst();
            REQUIRE(iter.Valid());
            REQUIRE(*(keys.begin()) == iter.key());

            iter.SeekToLast();
            REQUIRE(iter.Valid());
            REQUIRE(*(keys.rbegin()) == iter.key());
        }

        // Forward iteration test
        for (int i = 0; i < R; i++) {
            SkipList<Key, Comparator>::Iterator iter(&list);
            iter.Seek(i);

            // Compare against model iterator
            std::set<Key>::iterator model_iter = keys.lower_bound(i);
            for (int j = 0; j < 3; j++) {
                if (model_iter == keys.end()) {
                    REQUIRE(!iter.Valid());
                    break;
                } else {
                    REQUIRE(iter.Valid());
                    REQUIRE(*model_iter == iter.key());
                    ++model_iter;
                    iter.Next();
                }
            }
        }

        // Backward iteration test
        {
            SkipList<Key, Comparator>::Iterator iter(&list);
            iter.SeekToLast();

            // Compare against model iterator
            for (std::set<Key>::reverse_iterator model_iter = keys.rbegin();
                 model_iter != keys.rend();
                 ++model_iter) {
                REQUIRE(iter.Valid());
                REQUIRE(*model_iter == iter.key());
                iter.Prev();
            }
            REQUIRE(!iter.Valid());
        }
    }

    // We want to make sure that with a single writer and multiple
    // concurrent readers (with no synchronization other than when a
    // reader's iterator is created), the reader always observes all the
    // data that was present in the skip list when the iterator was
    // constructed.  Because insertions are happening concurrently, we may
    // also observe new values that were inserted since the iterator was
    // constructed, but we should never miss any values that were present
    // at iterator construction time.
    //
    // We generate multi-part keys:
    //     <key,gen,hash>
    // where:
    //     key is in range [0..K-1]
    //     gen is a generation number for key
    //     hash is hash(key,gen)
    //
    // The insertion code picks a random key, sets gen to be 1 + the last
    // generation number inserted for that key, and sets hash to Hash(key,gen).
    //
    // At the beginning of a read, we snapshot the last inserted
    // generation number for each key.  We then iterate, including random
    // calls to Next() and Seek().  For every key we encounter, we
    // check that it is either expected given the initial snapshot or has
    // been concurrently added since the iterator started.
} // namespace store
} // namespace leveldb