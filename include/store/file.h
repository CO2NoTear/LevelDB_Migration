////
// @file file.h
// @brief
// 定义文件接口
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#pragma once

#include <string>
#include "./status.h"

namespace store {

struct File
{
  public:
    File() = default;
    virtual ~File() = default;

    // Return a default environment suitable for the current operating
    // system.  Sophisticated users may wish to provide their own Env
    // implementation instead of relying on this default environment.
    //
    // The result of Default() belongs to leveldb and must never be deleted.
    static File *default_file();

    // Create an object that sequentially reads the file with the specified
    // name.
    // On success, stores a pointer to the new file in *result and returns OK.
    // On failure stores nullptr in *result and returns non-OK.  If the file
    // does not exist, returns a non-OK status.  Implementations should return a
    // NotFound status when the file does not exist.
    //
    // The returned file will only be accessed by one thread at a time.
    virtual Status create_sequential_file(
        const std::string &fname,
        SequentialFile **result) = 0;

  private:
    File(const File &) = delete;
    File &operator=(const File &) = delete;
};

} // namespace store
