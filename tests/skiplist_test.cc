// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <atomic>
#include <set>
#include <catch2/catch_test_macros.hpp>
#include "db/skiplist.h"
#include "util/arena.h"
#include "util/random.h"

#define ASSERT_TRUE(exp) REQUIRE(exp == true)
#define ASSERT_EQ(exp1, exp2) REQUIRE((exp1) == (exp2))

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
        ASSERT_TRUE(!list.Contains(10));

        SkipList<Key, Comparator>::Iterator iter(&list);
        ASSERT_TRUE(!iter.Valid());
        iter.SeekToFirst();
        ASSERT_TRUE(!iter.Valid());
        iter.Seek(100);
        ASSERT_TRUE(!iter.Valid());
        iter.SeekToLast();
        ASSERT_TRUE(!iter.Valid());
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
                ASSERT_EQ(keys.count(i), 1);
            } else {
                ASSERT_EQ(keys.count(i), 0);
            }
        }

        // Simple iterator tests
        {
            SkipList<Key, Comparator>::Iterator iter(&list);
            ASSERT_TRUE(!iter.Valid());

            iter.Seek(0);
            ASSERT_TRUE(iter.Valid());
            ASSERT_EQ(*(keys.begin()), iter.key());

            iter.SeekToFirst();
            ASSERT_TRUE(iter.Valid());
            ASSERT_EQ(*(keys.begin()), iter.key());

            iter.SeekToLast();
            ASSERT_TRUE(iter.Valid());
            ASSERT_EQ(*(keys.rbegin()), iter.key());
        }

        // Forward iteration test
        for (int i = 0; i < R; i++) {
            SkipList<Key, Comparator>::Iterator iter(&list);
            iter.Seek(i);

            // Compare against model iterator
            std::set<Key>::iterator model_iter = keys.lower_bound(i);
            for (int j = 0; j < 3; j++) {
                if (model_iter == keys.end()) {
                    ASSERT_TRUE(!iter.Valid());
                    break;
                } else {
                    ASSERT_TRUE(iter.Valid());
                    ASSERT_EQ(*model_iter, iter.key());
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
                ASSERT_TRUE(iter.Valid());
                ASSERT_EQ(*model_iter, iter.key());
                iter.Prev();
            }
            ASSERT_TRUE(!iter.Valid());
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