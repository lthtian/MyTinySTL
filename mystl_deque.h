#pragma once
#include "simple_alloc.h"
#include "stl_uninitialized.h"
#include "stl_construct.h"
#include "stl_iterator.h"
#include "type_traits.h"

inline size_t __deque_buf_size(size_t n, size_t sz)
{
	// 看是否自定义大小, 如果不是默认缓冲区大小为512, 容纳512 / sz个元素, 如果元素大小比512还大, 那么一个元素占用一个缓冲区
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

// deque迭代器
template<class T, class Ref, class Ptr, size_t BufSize>
struct __deque_iterator
{
	// 相关型别
	typedef random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef __deque_iterator<T, T&, T*, BufSize> iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSize> const_iterator;
	
	static size_t buffer_size() { return __deque_buf_size(BufSize, sizeof(T)); }

	typedef T** map_pointer;
	typedef __deque_iterator self; // self是内部定义的别名, 用来引用当前类的类型 , iterator是对外的, 用来创建迭代器对象

	// 迭代器成员变量
	T* cur;					// 当前节点真实所在
	T* first;				// 节点所在缓冲区左边界
	T* last;				// 右边界
	map_pointer node;		// 指向节点所在缓冲区的指针

	// 缓冲区跳跃, 但是不设置cur, cur需要根据实际情况调整
	void set_node(map_pointer new_node)
	{
		node = new_node;
		first = *new_node;
		// 这里时直接加一个缓冲区区间, 也就是last不属于缓冲区区间, 符合左闭右开
		last = first + difference_type(buffer_size()); 
	}

	//  运算符重载
	reference operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); }
	difference_type operator-(const self& x) const 
	{
		return (difference_type(buffer_size()) * (node - x.node - 1)) + (cur - first) + (x.last - x.cur);
	}

	self& operator++()
	{
		++cur;
		if (cur == last)
		{
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	
	self operator++(int)
	{
		self tmp = *this;
		++(*this);
		return tmp;
	}

	self& operator--()
	{
		if (cur == first)
		{
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}

	self operator--(int)
	{
		self tmp = *this;
		--(*this);
		return tmp;
	}

	self& operator+=(difference_type n)
	{
		difference_type offset = (cur - first) + n;

		if (offset >= 0 && offset < difference_type(buffer_size())) cur += n;  // 在同一缓冲区内
		else  // 不在同一缓冲区内 
		{
			difference_type node_offset =
				offset > 0 ? offset / difference_type(buffer_size())
				: - difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}

	self operator+(difference_type n) const
	{
		self tmp = *this;
		return tmp += n;
	}

	self& operator-=(difference_type n)
	{
		return *this += -n;
	}

	self operator-(difference_type n) const
	{
		self tmp = *this;
		return tmp -= n;
	}

	reference operator[](difference_type n) const { return *(*this + n); }
	bool operator==(const self& x) const { return cur == x.cur; }
	bool operator!=(const self& x) const { return !(*this == x); }
	bool operator<(const self& x) const
	{
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
};

template<class T, class Alloc = alloc, size_t BufSize = 0>
class deque
{
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef size_t difference_type;

	typedef __deque_iterator<T, T&, T*, BufSize> iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSize> const_iterator;


protected:
	typedef pointer* map_pointer;
	
protected:
	// 成员变量
	iterator start;				// 指向第一个缓冲区的第一个节点
	iterator finish;			// 指向最后一个缓冲区最后一个可用元素的下一个
	map_pointer map;			// 连续空间, 中控器
	size_type map_size;			// 记录map有多少个节点

public:
	// 基础机能
	iterator begin() { return start; }
	iterator end() { return finish; }
	reference operator[](size_type n) { return start[n]; }
	reference front() { return *start; }
	reference back() { iterator tmp = finish; --tmp; return *tmp; }
	//const_reference back() const { const_iterator tmp = finish; --tmp; return const_reference(*tmp); }
	size_type size() const { return finish - start;; }
	size_type max_size() const { return size_type(-1); }
	bool empty() const { return finish == start; }
	static size_t buffer_size() { return __deque_buf_size(BufSize, sizeof(T)); }

protected:
	// 构造与资源管理
	typedef simple_alloc<value_type, Alloc> data_allocator; // 元素空间配置器
	typedef simple_alloc<pointer, Alloc> map_allocator; // 指针空间配置器

	pointer allocate_node() { return data_allocator::allocate(buffer_size()); } // 用data_allocator申请一个缓冲区大小的空间
	void deallocate_node(pointer p) { data_allocator::deallocate(p); } // 释放

	void fill_initialize(size_type n, const value_type& x); // 安排deque结构的构造, 并设置初值
	void create_map_and_nodes(size_type num_elements); // 安排deque结构的部分
public:
	// 构造
	deque(int n, const value_type& x)
		:start(), finish(), map(nullptr), map_size(0)
	{
		fill_initialize(n, x);
	}
	
	deque() // 默认构造
	{
		map_size = 8;
		map = map_allocator::allocate(map_size);
		map_pointer nstart = map + 3;
		*nstart = allocate_node();
		start.set_node(nstart);
		start.cur = start.first;
		finish = start;
	}

public:
	// 插入
	void push_back(const value_type& x);
	void push_front(const value_type& x);
	void pop_back();
	void pop_front();
	void clear();		// 全部清空, 但是保留一个缓冲区
	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);
	iterator insert(iterator pos, const value_type& x);
	
private:
	void push_back_aux(const value_type& x);
	void push_front_aux(const value_type& x);
	void reserve_map_at_back(size_type nodes_to_add = 1);
	void reserve_map_at_front(size_type nodes_to_add = 1);
	void reallocate_map(size_type nodes_to_add, bool is_add_front);

	iterator insert_aux(iterator pos, const value_type& x);
};

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& x)
{
	create_map_and_nodes(n);
	map_pointer cur;
	try
	{
		for (cur = start.node; cur < finish.node; ++cur) 
			uninitalized_fill(*cur, *cur + buffer_size(), x);  // 为每个中控节点管理的缓冲区进行构造赋值
		uninitalized_fill(finish.first, finish.cur, x); // 后面的备用空间就不必初始化了 !!!
	}
	catch(...)
	{
		// rollback
	}
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
	// 根据申请的元素个数计算需要的中控节点数
	size_type nodes_to_get = num_elements / buffer_size() + 1;
	map_size = std::max(8, int(nodes_to_get + 2)); // 最终确定中控节点数
	map = map_allocator::allocate(map_size);
	// 将正在使用的中控节点控制在map中心
	map_pointer nstart = map + (map_size - nodes_to_get) / 2;
	map_pointer nfinish = nstart + nodes_to_get - 1;

	map_pointer cur;
	try
	{
		for (cur = nstart; cur <= nfinish; cur++)
			*cur = allocate_node();  // 为每个判定正在使用的中控节点申请管理的缓冲区
	}
	catch (...)
	{
		// rollback
	}

	// 初始化设定deque维护的两个迭代器
	start.set_node(nstart);
	start.cur = start.first; // 其实最后是默认start所在的缓冲区被填满了, 头插直接会造一个新缓冲区
	finish.set_node(nfinish);
	finish.cur = finish.first + num_elements % buffer_size();
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back(const value_type& x)
{
	if (finish.cur != finish.last - 1) // 如果当前缓冲区还未填满
	{
		construct(finish.cur, x); // 直接在当前区域构造
		++finish.cur;
	}
	else push_back_aux(x); // 需要修改finish指向一个新的缓冲区
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& x)
{
	value_type x_copy = x;
	reserve_map_at_back(); // 看看尾部还有没有中控节点, 没有就整改
	// 到这里尾部一定中控节点, 直接重新设定finish即可
	*(finish.node + 1) = allocate_node(); // 为新的中控节点申请一块缓冲区空间
	try
	{
		construct(finish.cur, x_copy); // ?
		finish.set_node(finish.node + 1);
		finish.cur = finish.first;
	}
	catch (...)
	{
		// rollback
	}
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front(const value_type& x)
{
	if (start.cur != start.first)
	{
		construct(start.cur - 1, x);
		--start.cur;
	}
	else push_front_aux(x);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& x)
{
	value_type x_copy = x;
	reserve_map_at_back(); 
	*(start.node - 1) = allocate_node(); // 为新的中控节点申请一块缓冲区空间
	try
	{
		start.set_node(start.node - 1);
		start.cur = start.last - 1;  // 当前指针设置为缓冲区最右端可用元素
		construct(start.cur, x_copy);
	}
	catch (...)
	{
		// rollback
	}
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_back(size_type nodes_to_add)
{
	if (nodes_to_add > map_size - (finish.node - map + 1)) // 先看看尾端数量够不够
		reallocate_map(nodes_to_add, false);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_front(size_type nodes_to_add)
{
	if (nodes_to_add > start.node - map)
		reallocate_map(nodes_to_add, true);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool is_add_front)
{
	size_type old_nodes_num = finish.node - start.node + 1;
	size_type new_nodes_num = old_nodes_num + nodes_to_add;

	map_pointer new_nstart;
	if (map_size > 2 * new_nodes_num) // 如果当前中控节点个数比需求个数还大两倍, 就不用扩容了
	{
		new_nstart = map + (map_size - new_nodes_num) / 2 + (is_add_front ? nodes_to_add : 0);
		// 为什么会有(is_add_front ? nodes_to_add : 0) ? 目的是在头插时将

		// 这样子是为了没有错误的拷贝
		if (new_nstart < start.node) std::copy(start.node, finish.node + 1, new_nstart);
		else std::copy_backward(start.node, finish.node + 1, new_nstart + old_nodes_num);
	}
	else
	{
		size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
		map_pointer new_map = map_allocator::allocate(new_map_size);
		new_nstart = new_map + (new_map_size - new_nodes_num) / 2 + (is_add_front ? nodes_to_add : 0);
		std::copy(start.node, finish.node + 1, new_nstart);
		map_allocator::deallocate(map, map_size);
		map = new_map;
		map_size = new_map_size;
	}

	start.set_node(new_nstart);
	finish.set_node(new_nstart + old_nodes_num - 1);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back()
{
	if (finish.cur != finish.first) // 当前缓冲区还有一个及以上
	{
		--finish.cur;
		destroy(finish.cur); // 析构该节点
	}
	else
	{
		deallocate_node(*(finish.node));  // 销毁该缓冲区
		finish.set_node(finish.node - 1);
		finish.cur = finish.last - 1;
		destroy(finish.cur);
	}
}


template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front()
{
	if (start.cur != start.last - 1) // 当前缓冲区有两个及以上
	{
		destroy(start.cur);
		++start.cur;
	}
	else // 只有一个
	{
		destroy(start.cur);
		deallocate_node(*(start.node));
		start.set_node(start.node + 1);
		start.cur = start.first;
	}
}


template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear()
{
	for (map_pointer cur = start.node + 1; cur < finish.node; ++cur)
	{
		destroy(*cur, *cur + buffer_size());
		deallocate_node(*cur);
	}

	if (start != finish)
	{
		destroy(start.cur, start.last);
		destroy(finish.first, finish.cur);
		deallocate_node(finish.first); // 删finish, 留start的
	}
	else destroy(start.cur, finish.cur);
	finish = start;
}


template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator 
deque<T, Alloc, BufSize>::erase(iterator pos)
{
	iterator next = pos;
	++next;
	difference_type index = pos - start;
	if (index < (size() >> 1))
	{
		std::copy_backward(start, pos, next);
		pop_front();
	}
	else
	{
		std::copy(next, finish, pos);
		pop_back();
	}
	return start + index;
}


template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
{
	if (first == start && last == finish)
	{
		clear();
		return finish;
	}
	
	difference_type n = last - first;
	difference_type elem_before = first - start;
	if (elem_before < (size() - n) / 2)
	{
		std::copy_backward(start, first, last);
		iterator new_start = start + n;
		destroy(start, new_start);
		for (map_pointer cur = start.node; cur < new_start.node; ++cur)
			deallocate_node(*cur);
		start = new_start;
	}
	else
	{
		std::copy(last, finish, first);
		iterator new_finish = finish - n;
		destroy(new_finish, finish);
		for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
			deallocate_node(*cur);
		finish = new_finish;
	}
	return start + elem_before;
}


template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert(iterator pos, const value_type& x)
{
	if (pos.cur == start.cur)
	{
		push_front(x);
		return start;
	}
	else if (pos.cur == finish.cur)
	{
		push_back(x);
		iterator tmp = finish;
		--tmp;
		return tmp;
	}
	else return insert_aux(pos, x);
}


template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x)
{
	difference_type index = pos - start;
	value_type x_copy = x;
	if (index < size() / 2)
	{
		push_front(front());
		iterator front1 = start;
		++front1;
		iterator front2 = front1;
		++front2;
		pos = start + index;
		iterator pos1 = pos;
		++pos1;
		std::copy(front2, pos1, front1);
	}
	else
	{
		push_back(back());
		iterator back1 = finish;
		--back1;
		iterator back2 = back1;
		--back2;
		pos = start + index;
		std::copy_backward(pos, back2, back1);
	}
	*pos = x_copy;
	return pos;
}