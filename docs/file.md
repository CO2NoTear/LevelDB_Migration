# file接口

levelDB使用一个Env接口来抽象文件相关的接口，在本系统中这个接口被重构为File接口。

```c++
struct File
{
  public:
    File() = default;
    virtual ~File() = default;
```