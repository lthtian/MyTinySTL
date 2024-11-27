#pragma once
#include"stl_iterator.h"

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void adjust_up(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp)
{
	Distance fa = (holeIndex - 1) / 2;
	while (holeIndex > topIndex && comp(*(first + fa), value)) // 父节点小于子节点, 父转到子
	{
		*(first + holeIndex) = *(first + fa);
		holeIndex = fa;
		fa = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
{
	adjust_up(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
}

template<class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__push_heap_aux(first, last, distance_type(first), value_type(first), comp);
}

// -----------------------------

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void adjust_down(RandomAccessIterator first, Distance fa, Distance len, T value, Compare comp)
{
	Distance topIndex = fa;
	Distance rchild = (fa * 2 + 1) + 1;  // 因为实际从0开始, 改为从1开始
	while (rchild < len)
	{
		if (comp(*(first + rchild), *(first + rchild - 1))) --rchild;
		*(first + fa) = *(first + rchild);
		fa = rchild;
		rchild = (fa * 2 + 1) + 1;
	}
	if (rchild == len) // 第len位不存在, 只有len - 1, 左节点
	{
		*(first + fa) = *(first + rchild - 1);
		fa = rchild - 1;
	}
	adjust_up(first, fa, topIndex, value, comp); // 为了适应反向的情况
}

// 将i是将要被割舍出来的值, 这个值要重新插入树中, 插入点就是顶端, 然后向下调整
template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator i, T value, Distance*, Compare comp)
{
	*i = *first;
	// 知道根节点, 下标起点, 节点个数, 原本最后一个尾节的值
	adjust_down(first, Distance(0), Distance(last - first), value, comp);
}

template<class RandomAccessIterator, class T, class Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp)
{
	__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first), comp);
}


template<class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__pop_heap_aux(first, last, value_type(first), comp);
}

// ----------------------------

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void __make_heap(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
{
	if (last - first < 2) return;
	Distance len = last - first;
	Distance fa = (len - 1 - 1) / 2; // 找出第一个需要重排的子树头部, 其实就是把后面一半的叶子节点跳过
	while (1) // 遍历所有子树, 都向下调整一遍, 可以保证是堆
	{
		adjust_down(first, fa, len, T(*(first + fa)), comp);
		if (fa == 0) return;
		fa--;
	}
}

template<class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__make_heap(first, last, distance_type(first), value_type(first), comp);
}