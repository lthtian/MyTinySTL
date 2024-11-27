#pragma once

// �Ƚ��� : �ṩһ��operator()�º���, ��������Ƚϴ�С����Ϊ

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