////
// @file status.h
// @brief
// 可解释错误
//
// @author uestc
// @email niexiaowen@uestc.edu.cn
//
#pragma once

#include <string.h>

namespace store {

class Status
{
  private:
    int status_; // 状态
    static const int user_code = -1000;

  public:
    Status()
        : status_(0)
    {}
    ~Status() = default;
    Status(const Status &other)
        : status_(other.status_)
    {}
    Status &operator=(const Status &other)
    {
        status_ = other.status_;
        return *this;
    }

    inline bool isError() { return status_ < 0; }
    inline bool isSystemError() { return status_ < 0 && status_ > user_code; }
    inline bool isLocalError() { return status_ <= user_code; }

    const char *descript()
    {
        if (status_ < 0 && status_ > user_code)
            return strerror(-status_);
        else if (status_ > user_code)
            return localDescript(-status_);
        else
            return localDescript(1000);
    }

    inline int get() { return status_; }
    inline void set(int code) { status_ = code; }

  private:
    const char *localDescript(int num);

    // enum Code
    // {
    //     kOk = 0,
    //     kNotFound = 1,
    //     kCorruption = 2,
    //     kNotSupported = 3,
    //     kInvalidArgument = 4,
    //     kIOError = 5
    // };
};

} // namespace store
