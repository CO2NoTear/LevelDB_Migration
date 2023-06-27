/**
 * @file memtableTest.cc
 * @author CO2NoTear (sqede@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <catch2/catch_test_macros.hpp>
#include "db/memtable.h"
#include "db/memtable.cc"
#include "db/dbformat.h"
#include "leveldb/table.h"
#include "util/status.cc"
#include <map>
#include <cstring>
#include <vector>

namespace leveldb {

namespace {
struct STLLessThan
{
    const Comparator *cmp;

    STLLessThan()
        : cmp(BytewiseComparator())
    {}
    STLLessThan(const Comparator *c)
        : cmp(c)
    {}
    bool operator()(const std::string &a, const std::string &b) const
    {
        return cmp->Compare(Slice(a), Slice(b)) < 0;
    }
};
} // namespace

typedef std::map<std::string, std::string, STLLessThan> KVMap;
typedef std::pair<std::string, std::string> KVItem;
InternalKeyComparator NewTestComparator()
{
    return InternalKeyComparator(BytewiseComparator());
}
Status FinishMemtable(MemTable *mem_, KVMap *data)
{
    mem_->Unref();
    mem_ = new MemTable(NewTestComparator());
    mem_->Ref();
    int seq = 1;
    for (const auto &kvp : *data) {
        mem_->Add(seq, kTypeValue, kvp.first, kvp.second);
        seq++;
        LookupKey lookupkey_(kvp.first, seq);
        std::string lookupvalue_;
        Status s;
        REQUIRE(mem_->Get(lookupkey_, &lookupvalue_, &s));
        REQUIRE(s.ok());
        REQUIRE(lookupvalue_ == kvp.second);
    }
    return Status::OK();
}
TEST_CASE("memtable")
{
    SECTION("add and get")
    {
        const InternalKeyComparator comparator_(BytewiseComparator());
        // InternalKey a = 1;
        MemTable *mem_ = new MemTable(comparator_);
        mem_->Ref();
        REQUIRE(
            strcmp(comparator_.Name(), "leveldb.InternalKeyComparator") == 0);
        KVMap *data = new KVMap;
        int seq = 1;
        const std::string testkey_ = "name";
        const std::string testvalue_ = "CO2NoTear";
        (*data)[testkey_] = testvalue_;
        mem_->Add(seq, kTypeValue, testkey_, testvalue_);
        LookupKey lookupkey_(testkey_, seq);
        Status s;
        std::string lookupvalue_;
        REQUIRE(mem_->Get(lookupkey_, &lookupvalue_, &s));
        REQUIRE(s.ok());
        REQUIRE(lookupvalue_ == testvalue_);
    }
    SECTION("finish")
    {
        KVMap *data = new KVMap;
        std::vector<KVItem> dataset;
        for (int seq = 1; seq <= 100; ++seq) {
            char *key_ = new char[20];
            std::snprintf(key_, 20, "%d's value", seq);
            std::string testkey_ = std::string(key_);
            char *value_ = new char[20];
            std::snprintf(key_, 20, "%d", seq);
            std::string testvalue_ = std::string(key_);
            delete[] key_;
            delete[] value_;
            dataset.push_back(KVItem(testkey_, testvalue_));
        }
        data->insert(dataset.begin(), dataset.end());

        MemTable *mem_ = new MemTable(NewTestComparator());
        mem_->Ref();

        int seq = 1;
        const std::string testkey_ = "name";
        const std::string testvalue_ = "CO2NoTear";
        mem_->Add(seq, kTypeValue, testkey_, testvalue_);
        LookupKey lookupkey_(testkey_, seq);
        Status s;
        std::string lookupvalue_;
        REQUIRE(mem_->Get(lookupkey_, &lookupvalue_, &s));
        REQUIRE(s.ok());
        REQUIRE(lookupvalue_ == testvalue_);

        FinishMemtable(mem_, data);
    }
} // namespace leveldb

} // namespace leveldb
