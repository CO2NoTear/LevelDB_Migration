// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <catch2/catch_test_macros.hpp>
#include "db/log_reader.h"
#include "db/log_writer.h"
#include "util/env_posix.cc"
#include "util/coding.h"
#include "util/crc32c.h"
#include "util/random.h"

namespace leveldb {
namespace log {
// 用指定字符串重复多次生成长度不小于n的大字符串
static std::string BigString(const std::string &partial_string, size_t n)
{
    std::string result;
    while (result.size() < n) {
        result.append(partial_string);
    }
    result.resize(n);
    return result;
}

TEST_CASE("writing")
{
    SECTION("short string")
    {
        PosixEnv *env_ = new PosixEnv;
        WritableFile *dest;
        SequentialFile *source;
        std::string msg = "hello,leveldb.";
        Status s = env_->NewWritableFile("testfile", &dest);
        REQUIRE(s.ok());
        Writer writer_(dest);
        REQUIRE(writer_.AddRecord(Slice(msg)).ok());
        s = env_->NewSequentialFile("testfile", &source);
        REQUIRE(s.ok());
        Reader::Reporter *repoter_;
        Reader reader_(source, repoter_, true, 0);
        Slice record;
        std::string scrach;
        REQUIRE(reader_.ReadRecord(&record, &scrach));
        REQUIRE(record.ToString() == msg);
    }
}

} // namespace log
} // namespace leveldb