#pragma once
/*
	提供五种迭代器类型
	提供萃取器获取相关型别
	提供一种迭代器移动策略, 一种迭代器距离测定策略
*/

#include <cstddef>

// 五种迭代器类型对应的标签类
	// 为什么要设置标签类? 为了利用函数重载机制使编译器在编译阶段就可以确定使用的函数, 大大提高效率
struct input_iterator_tag {};													// 顺序只读迭代器
struct output_iterator_tag {};													// 顺序只写迭代器
struct forward_iterator_tag : public input_iterator_tag {}; 					// 顺序读写迭代器
struct bidirectional_iterator_tag : public forward_iterator_tag {};				// 双向读写迭代器
struct random_access_iterator_tag : public bidirectional_iterator_tag {};		// 随机读写迭代器


// iterator类 标定五大迭代器相应型别(可以理解为迭代器特征)
// 为什么要有相应型别 ? 因为泛型算法中常常会用到这些型别, 所有迭代器在设计是应当自己显示定义这些型别
template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
struct iterator
{
	typedef Category	iterator_category;		// 迭代器种类
	typedef T			value_type;				// 迭代器所指之物的类型
	typedef Distance	difference_type;		// 迭代器距离类型
	typedef Pointer		pointer;				// 迭代器所指之物的指针
	typedef Reference	reference;				// 迭代器所指之物的引用
};

// traits 萃取器
// 为什么要有萃取器? 为了便于使用, 算法本身的接口常常只有迭代器和一些需求的数值
// 算法一开始是无法知道这些型别的, 就需要有手段可以仅仅通过迭代器就可以推知所有相关型别, 这种手段就是萃取器

template <class Iterator>
struct iterator_traits
{
	typedef typename Iterator::iterator_category	iterator_category;
	typedef typename Iterator::value_type			value_type;
	typedef typename Iterator::difference_type		difference_type;
	typedef typename Iterator::pointer				pointer;
	typedef typename Iterator::reference			reference;
};

// 萃取器对于原生指针设计的偏特化版
template <class T>
struct iterator_traits<T*>
{
	typedef typename random_access_iterator_tag		iterator_category;
	typedef typename T								value_type;
	typedef typename ptrdiff_t						difference_type;
	typedef typename T* pointer;
	typedef typename T& reference;
};

// 萃取器对于pointer to const设计的偏特化版
template <class T>
struct iterator_traits<const T*>
{
	typedef typename random_access_iterator_tag		iterator_category;
	typedef typename T								value_type;
	typedef typename ptrdiff_t						difference_type;
	typedef typename const T* pointer;
	typedef typename const T& reference;
};

// 直接利用迭代器和萃取器取出迭代器类型的泛型函数
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category	// 返回类型
iterator_category(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();
}

// 便于决定某个迭代器的 distance_type
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
}

// 便于决定某个迭代器的 value_type
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
}

// advance函数 (用于算法内部进行移动)
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
	while (n--) ++i;
}

template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, bidirectional_iterator_tag)
{
	if (n >= 0) while (n--) ++i;
	else while (n++) --i;
}

template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, random_access_iterator_tag)
{
	i += n;
}

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
	__advance(i, n, iterator_category(i));
}

// distance函数 (用于计算距离大小, 比如size(), count())
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
	// 使用类中嵌套类型必须使用typename
	typename iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last)
	{
		++first;
		++n;
	}
	return n;
}

template <class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
	return last - first;
}


template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
	return __distance(first, last, iterator_category(first));
}
