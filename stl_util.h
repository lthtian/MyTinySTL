#pragma once
#include <iostream>

template<class Ty1, class Ty2>
struct pair
{
	typedef Ty1 first_type;
	typedef Ty2 second_type;
	
	first_type first;
	second_type second;

	// 条件编译默认构造函数 
	// enable_if<bool, T> 使得某些类型的构造函数或函数只有在符合某些条件时才能被实例化
	// is_default_constructible<T>::value用来判断T类型是否有默认构造函数(无参)
	template <class Other1 = Ty1, class Other2 = Ty2, 
	typename = typename std::enable_if<
	std::is_default_constructible<Other1>::value &&
	std::is_default_constructible<Other2>::value, void>::type>
	constexpr pair()
		:first(), second()
	{}

	// 隐式可构造
	// is_copy_constructible<T> : 检查T是否可拷贝
	// is_convertible<const U&, T> : 检查某个类型是否能够通过 const T& 类型构造一个新的对象
	template <class U1 = Ty1, class U2 = Ty2,
		typename std::enable_if<
		std::is_copy_constructible<U1>::value&&
		std::is_copy_constructible<U2>::value&&
		std::is_convertible<const U1&, Ty1>::value&&
		std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
		constexpr pair(const Ty1& a, const Ty2& b)
		: first(a), second(b)
	{}

	// 显示可构造
	template <class U1 = Ty1, class U2 = Ty2,
		typename std::enable_if<
		std::is_copy_constructible<U1>::value&&
		std::is_copy_constructible<U2>::value &&
		(!std::is_convertible<const U1&, Ty1>::value ||
			!std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
	explicit constexpr pair(const Ty1& a, const Ty2& b)
		: first(a), second(b)
	{}

	// 默认生成拷贝构造和移动构造
	pair(const pair& rhs) = default;
	pair(pair&& rhs) = default;

	// 传入其他类型的隐式构造
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, Other1>::value&&
		std::is_constructible<Ty2, Other2>::value&&
		std::is_convertible<Other1&&, Ty1>::value&&
		std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
	constexpr pair(Other1&& a, Other2&& b)
		: first(std::forward<Other1>(a)), // 完美转发
		second(std::forward<Other2>(b))
	{}

	// 传入其他类型的显示构造
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, Other1>::value&&
		std::is_constructible<Ty2, Other2>::value &&
		(!std::is_convertible<Other1, Ty1>::value ||
			!std::is_convertible<Other2, Ty2>::value), int>::type = 0>
	explicit constexpr pair(Other1&& a, Other2&& b)
		: first(std::forward<Other1>(a)),
		second(std::forward<Other2>(b))
	{}

	// 传入其他pair的隐式构造
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, const Other1&>::value&&
		std::is_constructible<Ty2, const Other2&>::value&&
		std::is_convertible<const Other1&, Ty1>::value&&
		std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
	constexpr pair(const pair<Other1, Other2>& other)
		: first(other.first),
		second(other.second)
	{
	}

	// 传入其他pair的显示构造
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, const Other1&>::value&&
		std::is_constructible<Ty2, const Other2&>::value &&
		(!std::is_convertible<const Other1&, Ty1>::value ||
			!std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
	explicit constexpr pair(const pair<Other1, Other2>& other)
		: first(other.first),
		second(other.second)
	{
	}

	// implicit constructiable for other pair
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, Other1>::value&&
		std::is_constructible<Ty2, Other2>::value&&
		std::is_convertible<Other1, Ty1>::value&&
		std::is_convertible<Other2, Ty2>::value, int>::type = 0>
	constexpr pair(pair<Other1, Other2>&& other)
		: first(std::forward<Other1>(other.first)),
		second(std::forward<Other2>(other.second))
	{
	}

	// explicit constructiable for other pair
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, Other1>::value&&
		std::is_constructible<Ty2, Other2>::value &&
		(!std::is_convertible<Other1, Ty1>::value ||
			!std::is_convertible<Other2, Ty2>::value), int>::type = 0>
	explicit constexpr pair(pair<Other1, Other2>&& other)
		: first(std::forward<Other1>(other.first)),
		second(std::forward<Other2>(other.second))
	{
	}

	// 同类型pair的左值赋值重载
	pair& operator=(const pair& rhs)
	{
		if (this != &rhs)
		{
			first = rhs.first;
			second = rhs.second;
		}
		return *this;
	}

	// 同类型pair的右值值赋值重载
	pair& operator=(pair&& rhs)
	{
		if (this != &rhs)
		{
			first = std::move(rhs.first);
			second = std::move(rhs.second);
		}
		return *this;
	}

	// 不同类型pair的左值赋值重载
	template <class Other1, class Other2>
	pair& operator=(const pair<Other1, Other2>& other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	// 不同类型pair的右值值赋值重载
	template <class Other1, class Other2>
	pair& operator=(pair<Other1, Other2>&& other)
	{
		first = std::forward<Other1>(other.first);
		second = std::forward<Other2>(other.second);
		return *this;
	}

	~pair() = default;

	void swap(pair& other)
	{
		if (this != &other)
		{
			std::swap(first, other.first);
			std::swap(second, other.second);
		}
	}

};

// 重载比较操作符 
template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
	return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
	return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class Ty1, class Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
	return !(lhs == rhs);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
	return rhs < lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
	return !(rhs < lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
	return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
{
	lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个 pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
{
	return pair<Ty1, Ty2>(std::forward<Ty1>(first), std::forward<Ty2>(second));
}
