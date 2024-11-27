#pragma once
#include "simple_alloc.h"
#include "stl_uninitialized.h"
#include "stl_construct.h"
#include "stl_iterator.h"
#include "type_traits.h"

inline size_t __deque_buf_size(size_t n, size_t sz)
{
	// ���Ƿ��Զ����С, �������Ĭ�ϻ�������СΪ512, ����512 / sz��Ԫ��, ���Ԫ�ش�С��512����, ��ôһ��Ԫ��ռ��һ��������
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

// deque������
template<class T, class Ref, class Ptr, size_t BufSize>
struct __deque_iterator
{
	// ����ͱ�
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
	typedef __deque_iterator self; // self���ڲ�����ı���, �������õ�ǰ������� , iterator�Ƕ����, ������������������

	// ��������Ա����
	T* cur;					// ��ǰ�ڵ���ʵ����
	T* first;				// �ڵ����ڻ�������߽�
	T* last;				// �ұ߽�
	map_pointer node;		// ָ��ڵ����ڻ�������ָ��

	// ��������Ծ, ���ǲ�����cur, cur��Ҫ����ʵ���������
	void set_node(map_pointer new_node)
	{
		node = new_node;
		first = *new_node;
		// ����ʱֱ�Ӽ�һ������������, Ҳ����last�����ڻ���������, ��������ҿ�
		last = first + difference_type(buffer_size()); 
	}

	//  ���������
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

		if (offset >= 0 && offset < difference_type(buffer_size())) cur += n;  // ��ͬһ��������
		else  // ����ͬһ�������� 
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
	// ��Ա����
	iterator start;				// ָ���һ���������ĵ�һ���ڵ�
	iterator finish;			// ָ�����һ�����������һ������Ԫ�ص���һ��
	map_pointer map;			// �����ռ�, �п���
	size_type map_size;			// ��¼map�ж��ٸ��ڵ�

public:
	// ��������
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
	// ��������Դ����
	typedef simple_alloc<value_type, Alloc> data_allocator; // Ԫ�ؿռ�������
	typedef simple_alloc<pointer, Alloc> map_allocator; // ָ��ռ�������

	pointer allocate_node() { return data_allocator::allocate(buffer_size()); } // ��data_allocator����һ����������С�Ŀռ�
	void deallocate_node(pointer p) { data_allocator::deallocate(p); } // �ͷ�

	void fill_initialize(size_type n, const value_type& x); // ����deque�ṹ�Ĺ���, �����ó�ֵ
	void create_map_and_nodes(size_type num_elements); // ����deque�ṹ�Ĳ���
public:
	// ����
	deque(int n, const value_type& x)
		:start(), finish(), map(nullptr), map_size(0)
	{
		fill_initialize(n, x);
	}
	
	deque() // Ĭ�Ϲ���
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
	// ����
	void push_back(const value_type& x);
	void push_front(const value_type& x);
	void pop_back();
	void pop_front();
	void clear();		// ȫ�����, ���Ǳ���һ��������
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
			uninitalized_fill(*cur, *cur + buffer_size(), x);  // Ϊÿ���пؽڵ����Ļ��������й��츳ֵ
		uninitalized_fill(finish.first, finish.cur, x); // ����ı��ÿռ�Ͳ��س�ʼ���� !!!
	}
	catch(...)
	{
		// rollback
	}
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
{
	// ���������Ԫ�ظ���������Ҫ���пؽڵ���
	size_type nodes_to_get = num_elements / buffer_size() + 1;
	map_size = std::max(8, int(nodes_to_get + 2)); // ����ȷ���пؽڵ���
	map = map_allocator::allocate(map_size);
	// ������ʹ�õ��пؽڵ������map����
	map_pointer nstart = map + (map_size - nodes_to_get) / 2;
	map_pointer nfinish = nstart + nodes_to_get - 1;

	map_pointer cur;
	try
	{
		for (cur = nstart; cur <= nfinish; cur++)
			*cur = allocate_node();  // Ϊÿ���ж�����ʹ�õ��пؽڵ��������Ļ�����
	}
	catch (...)
	{
		// rollback
	}

	// ��ʼ���趨dequeά��������������
	start.set_node(nstart);
	start.cur = start.first; // ��ʵ�����Ĭ��start���ڵĻ�������������, ͷ��ֱ�ӻ���һ���»�����
	finish.set_node(nfinish);
	finish.cur = finish.first + num_elements % buffer_size();
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back(const value_type& x)
{
	if (finish.cur != finish.last - 1) // �����ǰ��������δ����
	{
		construct(finish.cur, x); // ֱ���ڵ�ǰ������
		++finish.cur;
	}
	else push_back_aux(x); // ��Ҫ�޸�finishָ��һ���µĻ�����
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& x)
{
	value_type x_copy = x;
	reserve_map_at_back(); // ����β������û���пؽڵ�, û�о�����
	// ������β��һ���пؽڵ�, ֱ�������趨finish����
	*(finish.node + 1) = allocate_node(); // Ϊ�µ��пؽڵ�����һ�黺�����ռ�
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
	*(start.node - 1) = allocate_node(); // Ϊ�µ��пؽڵ�����һ�黺�����ռ�
	try
	{
		start.set_node(start.node - 1);
		start.cur = start.last - 1;  // ��ǰָ������Ϊ���������Ҷ˿���Ԫ��
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
	if (nodes_to_add > map_size - (finish.node - map + 1)) // �ȿ���β������������
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
	if (map_size > 2 * new_nodes_num) // �����ǰ�пؽڵ���������������������, �Ͳ���������
	{
		new_nstart = map + (map_size - new_nodes_num) / 2 + (is_add_front ? nodes_to_add : 0);
		// Ϊʲô����(is_add_front ? nodes_to_add : 0) ? Ŀ������ͷ��ʱ��

		// ��������Ϊ��û�д���Ŀ���
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
	if (finish.cur != finish.first) // ��ǰ����������һ��������
	{
		--finish.cur;
		destroy(finish.cur); // �����ýڵ�
	}
	else
	{
		deallocate_node(*(finish.node));  // ���ٸû�����
		finish.set_node(finish.node - 1);
		finish.cur = finish.last - 1;
		destroy(finish.cur);
	}
}


template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front()
{
	if (start.cur != start.last - 1) // ��ǰ������������������
	{
		destroy(start.cur);
		++start.cur;
	}
	else // ֻ��һ��
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
		deallocate_node(finish.first); // ɾfinish, ��start��
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