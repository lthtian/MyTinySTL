#pragma once
#include <iostream>

template<class Ty1, class Ty2>
struct pair
{
	typedef Ty1 first_type;
	typedef Ty2 second_type;
	
	first_type first;
	second_type second;

	// ��������Ĭ�Ϲ��캯�� 
	// enable_if<bool, T> ʹ��ĳЩ���͵Ĺ��캯������ֻ���ڷ���ĳЩ����ʱ���ܱ�ʵ����
	// is_default_constructible<T>::value�����ж�T�����Ƿ���Ĭ�Ϲ��캯��(�޲�)
	template <class Other1 = Ty1, class Other2 = Ty2, 
	typename = typename std::enable_if<
	std::is_default_constructible<Other1>::value &&
	std::is_default_constructible<Other2>::value, void>::type>
	constexpr pair()
		:first(), second()
	{}

	// ��ʽ�ɹ���
	// is_copy_constructible<T> : ���T�Ƿ�ɿ���
	// is_convertible<const U&, T> : ���ĳ�������Ƿ��ܹ�ͨ�� const T& ���͹���һ���µĶ���
	template <class U1 = Ty1, class U2 = Ty2,
		typename std::enable_if<
		std::is_copy_constructible<U1>::value&&
		std::is_copy_constructible<U2>::value&&
		std::is_convertible<const U1&, Ty1>::value&&
		std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
		constexpr pair(const Ty1& a, const Ty2& b)
		: first(a), second(b)
	{}

	// ��ʾ�ɹ���
	template <class U1 = Ty1, class U2 = Ty2,
		typename std::enable_if<
		std::is_copy_constructible<U1>::value&&
		std::is_copy_constructible<U2>::value &&
		(!std::is_convertible<const U1&, Ty1>::value ||
			!std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
	explicit constexpr pair(const Ty1& a, const Ty2& b)
		: first(a), second(b)
	{}

	// Ĭ�����ɿ���������ƶ�����
	pair(const pair& rhs) = default;
	pair(pair&& rhs) = default;

	// �����������͵���ʽ����
	template <class Other1, class Other2,
		typename std::enable_if<
		std::is_constructible<Ty1, Other1>::value&&
		std::is_constructible<Ty2, Other2>::value&&
		std::is_convertible<Other1&&, Ty1>::value&&
		std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
	constexpr pair(Other1&& a, Other2&& b)
		: first(std::forward<Other1>(a)), // ����ת��
		second(std::forward<Other2>(b))
	{}

	// �����������͵���ʾ����
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

	// ��������pair����ʽ����
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

	// ��������pair����ʾ����
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

	// ͬ����pair����ֵ��ֵ����
	pair& operator=(const pair& rhs)
	{
		if (this != &rhs)
		{
			first = rhs.first;
			second = rhs.second;
		}
		return *this;
	}

	// ͬ����pair����ֵֵ��ֵ����
	pair& operator=(pair&& rhs)
	{
		if (this != &rhs)
		{
			first = std::move(rhs.first);
			second = std::move(rhs.second);
		}
		return *this;
	}

	// ��ͬ����pair����ֵ��ֵ����
	template <class Other1, class Other2>
	pair& operator=(const pair<Other1, Other2>& other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	// ��ͬ����pair����ֵֵ��ֵ����
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

// ���رȽϲ����� 
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

// ���� mystl �� swap
template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
{
	lhs.swap(rhs);
}

// ȫ�ֺ��������������ݳ�Ϊһ�� pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
{
	return pair<Ty1, Ty2>(std::forward<Ty1>(first), std::forward<Ty2>(second));
}
