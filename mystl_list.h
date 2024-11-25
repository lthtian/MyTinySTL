#pragma once
#include "simple_alloc.h"
#include "stl_uninitialized.h"
#include "stl_construct.h"
#include "stl_iterator.h"

template<class T>
struct __list_node
{
	typedef __list_node<T>* pointer;
	pointer prev;
	pointer next;
	T data;
};

// 提供Ref和Ptr更多是为了匹配const版本
// list的迭代器类型是双向迭代器
template<class T, class Ref, class Ptr>
struct __list_iterator
{
	typedef __list_iterator<T, T&, T*> iterator;
	typedef __list_iterator<T, Ref, Ptr> self;

	// 声明自己是双向迭代器
	typedef bidirectional_iterator_tag iterator_category;

	// 先确定成员变量是一个指向__list_node的指针
	typedef __list_node<T>* link_type;
	link_type node;

	// 五大相关型别
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	// 迭代器构造函数
	__list_iterator(link_type x) :node(x) {}
	__list_iterator() {}
	__list_iterator(const iterator& x) :node(x.node) {}

	// 迭代器运算符重载
	bool operator== (const iterator& x) const { return node == x.node; }
	bool operator!= (const iterator& x) const { return node != x.node; }
	reference operator* () const { return node->data; }
	reference operator-> () const { return &(operator*()); }

	self& operator++()
	{
		node = node->next;
		return *this;
	}

	self operator++(int) // 后置只能传值, 传引用会有问题
	{
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--()
	{
		node = node->prev;
		return *this;
	}
	
	self operator--(int)
	{
		self temp = *this;
		--*this;
		return temp;
	}
};

template<class T, class Alloc = alloc>
class list
{
protected:
	typedef __list_node<T> list_node;
	typedef __list_iterator<T, T&, T*> iterator;

public:
	typedef list_node* link_type;

	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

protected:
	// 一个链表实际只需维护一个指向尾节点的指针, 这个节点是空白节点, 用于符合前闭后开的原则
	link_type node;

public:
	// 依据node就可以维护简单函数
	iterator begin() { return node->next; }  // 其实是用返回的link_type类型调用iterator的构造函数再返回iterator
	iterator end() { return node; }
	bool empty() const { return node->next == node; }
	size_type size() const { return distance(begin(), end()); }
	reference front() { return *begin(); }
	reference back() { return *(--end()); }

protected:
	// 引入空间配置器及其功能函数
	typedef simple_alloc<list_node, Alloc> list_node_allocator;
	
	link_type get_node() { return list_node_allocator::allocate(); }		// 配置节点
	void put_node(link_type p) { list_node_allocator::deallocate(p); }		// 销毁节点
	
	link_type create_node(const T& x)	// 配置并构造节点
	{
		link_type ret = get_node();
		construct(&ret->data, x);
		return ret;
	}

	void destroy_node(link_type p)		// 析构并销毁节点
	{
		destroy(&p->data);
		put_node(p);
	}

public:
	// list构造
	list() { empty_initialize(); }

protected:
	void empty_initialize()
	{
		node = get_node();
		node->next = node;
		node->prev = node;
	}

public:
	// CURD动作
	iterator insert(iterator pos, const T& x); // 在pos位置插入一个节点, 内容为x, 返回插入节点的迭代器
	void push_back(const T& x) { insert(end(), x); }
	void push_front(const T& x) { insert(begin(), x); }
	iterator erase(iterator pos);	// 删除pos位置的节点, 返回删除节点后第一个没被删的
	void pop_back() { erase(--end()); }
	void pop_front() { erase(begin()); }
	void clear(); 
	void remove(const T& x);   // 删除所有值为x的节点
	void unique();			   // 删除所有连续相同的节点
	void transfer(iterator pos, iterator first, iterator last); // 将[first, last)移至pos前
	void splice(iterator pos, list& x);							// 将链表x接合于指定位置之前
	void splice(iterator pos, list& x, iterator i);				//将单个i接到指定位置之前
	void splice(iterator pos, list& x, iterator first, iterator last);   // 将[first, last)移至pos前 
	void merge(list& x);		// 将x合并到*this中, 必须提前排过序
	void reverse();				// 逆置
	void sort();				// 排序
	void swap(list& x);			// 交换	
};


// 具体实现

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator pos, const T& x)
{
	link_type newnode = create_node(x);
	newnode->next = pos.node;
	newnode->prev = pos.node->prev;
	pos.node->prev->next = newnode;
	pos.node->prev = newnode;
	return newnode;
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator pos)
{
	pos.node->prev->next = pos.node->next;
	pos.node->next->prev = pos.node->prev;
	link_type ret = pos.node->next;
	destroy_node(pos.node);
	return iterator(ret);
}

template<class T, class Alloc>
void list<T, Alloc>::clear()
{
	link_type cur = node->next;
	while (cur != node)
	{
		link_type tmp = cur;
		cur = cur->next;
		destroy_node(tmp);
	}
	node->next = node;
	node->prev = node;
}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T& x)
{
	link_type cur = node->next;
	while (cur != node)
	{
		link_type tmp = cur;
		cur = cur->next;
		if (tmp->data == x) erase(iterator(tmp)); // 为了使用erase, 通过构造上升到迭代器
	}
}

template<class T, class Alloc>
void list<T, Alloc>::unique()
{
	link_type cur = node->next;
	while (cur != node)
	{
		link_type tmp = cur;
		cur = cur->next;
		if (tmp->data == cur->data) erase(iterator(tmp));
	}
}

template<class T, class Alloc>
void list<T, Alloc>::transfer(iterator pos, iterator first, iterator last)
{
	if (pos == last) return;
	last.node->prev->next = pos.node;
	first.node->prev->next = last.node;
	pos.node->prev->next = first.node;
	link_type tmp = pos.node->prev;
	pos.node->prev = last.node->prev;
	last.node->prev = first.node->prev;
	first.node->prev = tmp;
}


template<class T, class Alloc>
void list<T, Alloc>::splice(iterator pos, list& x)
{
	if (!x.empty()) transfer(pos, x.begin(), x.end());
}

template<class T, class Alloc>
void list<T, Alloc>::splice(iterator pos, list& x, iterator i)
{
	iterator j = i;
	++j;
	if (pos == i || pos == j) return;
	transfer(pos, i, j);
}

template<class T, class Alloc>
void list<T, Alloc>::splice(iterator pos, list& x, iterator first, iterator last)
{
	if (first == last) return;
	transfer(pos, first, last);
}

template<class T, class Alloc>
void list<T, Alloc>::merge(list& x)
{
	iterator first1 = begin();
	iterator first2 = x.begin();
	iterator last1 = end();
	iterator last2 = x.end();

	while (first1 != last1 && first2 != last2)
	{
		if (*first1 > *first2)
		{
			iterator tmp = first2;
			transfer(first1, first2, ++tmp);
			first2 = tmp;
		}
		else first1++;
	}
	if (first2 != last2) transfer(last1, first2, last2);
}

template<class T, class Alloc>
void list<T, Alloc>::reverse()
{
	if (node->next == node || node->next->next == node) return;
	iterator first = begin();
	++first;
	while (first != end())
	{
		iterator old = first;
		transfer(begin(), first, ++old);
		first = old;
	}
}


// 非递归归并排序
template<class T, class Alloc>
void list<T, Alloc>::sort()
{
	if (node->next == node || node->next->next == node) return;
	list<T, Alloc> carry;
	list<T, Alloc> counter[64];
	int fill = 0;
	while (!empty())
	{
		carry.splice(carry.begin(), *this, begin());
		int i = 0;
		while (i < fill && !counter[i].empty())
		{
			counter[i].merge(carry);
			carry.swap(counter[i++]);
		}
		carry.swap(counter[i]);
		if (i == fill) ++fill;
	}
	for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1]);
	swap(counter[fill - 1]);
}

template<class T, class Alloc>
void list<T, Alloc>::swap(list& x)
{
	link_type tmp = node;
	node = x.node;
	x.node = tmp;
}