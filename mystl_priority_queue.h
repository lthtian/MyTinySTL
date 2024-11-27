#pragma once
#include "mystl_vector.h"
#include "mystl_heap.h"
#include "stl_comparator.h"

namespace lth
{
	template<class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type>>
	class priority_queue
	{
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::const_reference const_reference;

	protected:
		Sequence c;
		Compare comp; // 这个变量用于使用仿函数来比较大小

	public:
		// 构造
		priority_queue() : c() {}
		// 为什么要传比较器变量进来?
		// 当自定义的比较器需要接受参数时, 应当创建一个比较器变量输入参数改变比较器的性质, 然后传入此变量实现不同的效果
		explicit priority_queue(const Compare& x) :c() ,comp(x) {} 


		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last, const Compare& x)
			: c(first, last), comp(x)
		{
			make_heap(c.begin(), c.end(), comp);
		}

		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last)
			: c(first, last)
		{
			make_heap(c.begin(), c.end(), comp);
		}


	public:
		// 基础机能
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference top() { return c.front(); }

		void push(const value_type& x) 
		{
			c.push_back(x);
			push_heap(c.begin(), c.end(), comp);
		}

		void pop() 
		{ 
			pop_heap(c.begin(), c.end(), comp);
			c.pop_back(); 
		}
	};

}