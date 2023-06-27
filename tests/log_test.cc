// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
/**
 * @file log_test.cc
 * @author CO2NoTear (sqede@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-06-26
 *
 * @copyright Copyright (c) 2023
 *
 */

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

static std::string NumberString(int x)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%d.", x);
    return std::string(buf);
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
    SECTION("long string")
    {
        // 观察是否有分块
        PosixEnv *env_ = new PosixEnv;
        WritableFile *dest;
        SequentialFile *source;
        std::string msg = BigString("hello ", 102400);
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
    SECTION("many blocks")
    {
        // 生成1w个数字组成的字符串，全部写进log
        PosixEnv *env_ = new PosixEnv;
        WritableFile *dest;
        SequentialFile *source;
        Status s = env_->NewWritableFile("testfile", &dest);
        REQUIRE(s.ok());
        Writer writer_(dest);
        for (int i = 1; i <= 100000; ++i) {
            std::string msg = NumberString(i);
            REQUIRE(writer_.AddRecord(Slice(msg)).ok());
        }
        s = env_->NewSequentialFile("testfile", &source);
        REQUIRE(s.ok());
        Reader::Reporter *repoter_;
        Reader reader_(source, repoter_, true, 0);
        Slice record;
        std::string scrach;
        for (int i = 1; i <= 100000; ++i) {
            std::string msg = NumberString(i);
            REQUIRE(reader_.ReadRecord(&record, &scrach));
            REQUIRE(record.ToString() == msg);
        }
        // EOF
        REQUIRE(!reader_.ReadRecord(&record, &scrach));
    }
    SECTION("aligned EOF")
    {
        // 填充整个block直到只剩下4字节，使得最后部分被00填充
        const int n = kBlockSize - 2 * kHeaderSize + 3;
        PosixEnv *env_ = new PosixEnv;
        WritableFile *dest;
        SequentialFile *source;
        std::string msg = BigString("hello", n);
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
        // EOF read
        REQUIRE(!reader_.ReadRecord(&record, &scrach));
    }
}

} // namespace log
} // namespace leveldb