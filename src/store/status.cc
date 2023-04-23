////
// @file status.cc
// @brief
// 系统错误码
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#include <algorithm>
#include <store/status.h>

namespace store {

struct LocalStatus
{
    int num;
    const char *description;
};

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
    if (ret == locals_ + size)
        return nullptr;
    else
        return ret->description;
}

} // namespace store