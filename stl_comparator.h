#pragma once

// 比较器 : 提供一个operator()仿函数, 用来代替比较大小的行为

template <class T>
struct less {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

template <class T>
struct greater {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};