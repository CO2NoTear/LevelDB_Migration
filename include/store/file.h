////
// @file file.h
// @brief
// 定义文件接口
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#ifndef __Fuchsia__
#    include <sys/resource.h>
#endif
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

#include <string>
#include "./status.h"
#include "./slice.h"

namespace store {

class SequentialFile;
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

class SequentialFile
{
  private:
    const int fd_;
    const std::string filename_;

  public:
    SequentialFile() = default;

    SequentialFile(const SequentialFile &) = delete;
    SequentialFile &operator=(const SequentialFile &) = delete;

    virtual ~SequentialFile();

    // Read up to "n" bytes from the file.  "scratch[0..n-1]" may be
    // written by this routine.  Sets "*result" to the data that was
    // read (including if fewer than "n" bytes were successfully read).
    // May set "*result" to point at data in "scratch[0..n-1]", so
    // "scratch[0..n-1]" must be live when "*result" is used.
    // If an error was encountered, returns a non-OK status.
    //
    // REQUIRES: External synchronization
    virtual Status Read(size_t n, Slice *result, char *scratch) = 0;

    // Skip "n" bytes from the file. This is guaranteed to be no
    // slower that reading the same data, but may be faster.
    //
    // If end of file is reached, skipping will stop at the end of the
    // file, and Skip will return OK.
    //
    // REQUIRES: External synchronization
    virtual Status Skip(uint64_t n) = 0;

    Status Skip(uint64_t n)
    {
        if (::lseek(fd_, n, SEEK_CUR) == static_cast<off_t>(-1)) {
            return Status(-1);
        }
        return Status();
    }
};

} // namespace store
