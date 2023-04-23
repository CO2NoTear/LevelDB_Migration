# status可解释错误

Status类封装返回状态。当出错时，Linux的系统返回值为-1，然后在errno上设置一个错误值。

```c++
class Status
{
  private:
    int status_; // 状态
```

当status_ >= 0，表示返回值正确，比如read()/write()函数正常读写，status_ < 0表示出错，此时-status_对应于系统的errno值。

检查系统的errno，Linux的errno < 1000，所以当-1000< status_ < 0表示系统故障，而status_ < -1000表示定制故障。

系统内部的错误在Status::localDescript()函数中定义

```c++
const char *Status::localDescript(int num)
{
    // TODO: 系统局部的错误编码，1000+
    static const LocalStatus locals_[] = {
        {1000, "local error"},
        {0, nullptr},
    };
    static const int size = sizeof(locals_) / sizeof(LocalStatus);

    const LocalStatus *ret = std::lower_bound(
        &locals_[0],
        &locals_[size],
        num,
        [](const LocalStatus &d, int i) -> bool { return i < d.num; });
    ...
```