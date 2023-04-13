////
// @file arena.h
// @brief
// Arena分配器
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#pragma once

#include <assert.h>
#include <atomic>
#include <vector>

namespace store {

class Arena
{
  private:
    // Allocation state
    char *alloc_ptr_;
    size_t alloc_bytes_remaining_;

    // Array of new[] allocated memory blocks
    std::vector<char *> blocks_;

    // Total memory usage of the arena.
    //
    // TODO(costan): This member is accessed via atomics, but the others are
    //               accessed without any locking. Is this OK?
    std::atomic<size_t> memory_usage_;

  public:
    Arena();

    ~Arena();

    // Return a pointer to a newly allocated memory block of "bytes" bytes.
    char *allocate(size_t bytes);

    // Allocate memory with the normal alignment guarantees provided by malloc.
    char *aligned(size_t bytes);

    // Returns an estimate of the total memory usage of data allocated
    // by the arena.
    size_t usage() const
    {
        return memory_usage_.load(std::memory_order_relaxed);
    }

  private:
    Arena(const Arena &) = delete;
    Arena &operator=(const Arena &) = delete;

    char *fallback(size_t bytes);
    char *newblock(size_t block_bytes);
};

inline char *Arena::allocate(size_t bytes)
{
    // The semantics of what to return are a bit messy if we allow
    // 0-byte allocations, so we disallow them here (we don't need
    // them for our internal use).
    assert(bytes > 0);
    if (bytes <= alloc_bytes_remaining_) {
        char *result = alloc_ptr_;
        alloc_ptr_ += bytes;
        alloc_bytes_remaining_ -= bytes;
        return result;
    }
    return fallback(bytes);
}

} // namespace store