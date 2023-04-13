# arena分配器

对象从Arena中分配，但不回收，只有在Arena销毁时，所有内存一次性被回收。

```c++
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
```

TODO: Arena详细原理？