# slice

slice是levelDB里一个基础数据结构，它用于封装key、value。slice内部结构与std::string基本一致，这意味着你将slice强转成std::string，多半是可用的。

```c++
class Slice
{
  private:
    const char *data_; // 指向数据
    size_t size_;      // 数据长度
```

slice内部的比较函数值得注意，它是一个字典序比较函数，与key、value的类型无关。在分析levelDB代码中，需要关注levelDB的key、value是否有类型。

```c++
inline int Slice::compare(const Slice &b) const
{
    const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
    int r = ::memcmp(data_, b.data_, min_len);
    ...
}
```

slice构造和析构函数没有分配和析构数据，这与它的应用场景相关。