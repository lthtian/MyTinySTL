#pragma once
#include "simple_alloc.h"
#include "stl_iterator.h"
#include "stl_construct.h"
#include "stl_comparator.h"
#include "stl_uninitialized.h"

// 节点颜色
typedef bool __rb_tree_color_type;
const __rb_tree_color_type red = false;
const __rb_tree_color_type black = true;

// 节点设置
struct __rb_tree_node_base
{
	typedef __rb_tree_color_type color_type;
	typedef __rb_tree_node_base* base_ptr;

	color_type color;
	base_ptr parent;
	base_ptr left;
	base_ptr right;

	static base_ptr minimum(base_ptr x)
	{
		while (x->left != 0) x = x->left;
		return x;
	}

	static base_ptr maximum(base_ptr x)
	{
		while (x->right != 0) x = x->right;
		return x;
	}
};

// 内层实现指针结构(和迭代器的实际移动联动), 外层管理类型和数据(和迭代器的重载联动)
template<class T>
struct __rb_tree_node : public __rb_tree_node_base
{
	typedef __rb_tree_node<T>* link_type; 
	T data;
};


// 迭代器
struct __rb_tree_iterator_base
{
	typedef __rb_tree_node_base::base_ptr base_ptr;
	typedef bidirectional_iterator_tag iterator_category; // 声明双向迭代器
	typedef ptrdiff_t difference_type;

	base_ptr node;  // 与节点的唯一必要联系

	// 底层迭代器移动 -- 中序遍历 + header特殊状况判断
	void increment()
	{
		if (node->right != 0) // 右边还有子节点, 此情况最终会移动到右的最左
		{
			node = node->right;
			// 一路向左
			while (node->left != nullptr) node = node->left;
		}
		else // 右边无子节点, 往上找父节点
		{
			base_ptr fa = node->parent;
			// 往上找的父节点不能是右节点, 如果是右节点说明肯定已经遍历过了, 往上回溯
			while (fa->right == node)
			{
				node = fa;
				fa = node->parent;
			}

			if (node->right != fa) node = fa;
			// 须知前提是root和header互为parent
			// 只有在走到end时, 才会一路回溯向上回溯到root和header
			// 最后的结果是node赋值为header, fa赋值为root, 有且仅有此时node->right = fa
			// 当node->right = fa时, 应当给出end(), 也就是header, 也就是node, 因此不变
			// 当 != 时, 就是正常找法, 下一个节点应当是fa, 所以node = fa;
		}
	}

	void decrement()
	{
		// 此情况发生在node为header时, header = end(), 所以--应当是最右节点
		if (node->color == red && node->parent->parent == node) node = node->right;
		else if (node->left != 0) // 其他都是上面的镜像操作
		{
			base_ptr fa = node->left;
			while (fa->right != nullptr) fa = fa->right;
			node = fa;
		}
		else
		{
			base_ptr fa = node->parent;
			while (node == fa->left)
			{
				node = fa;
				fa = fa->parent;
			}
			node = fa;
		}
	}

};

template <class T, class Ref, class Ptr >
struct __rb_tree_iterator : public __rb_tree_iterator_base
{
	typedef T value_type;
	typedef Ref reference;
	typedef Ptr pointer;
	typedef __rb_tree_iterator<T, T&, T*> iterator;
	typedef __rb_tree_iterator<T, const T&, const T*> const_iterator;
	typedef __rb_tree_iterator<T, Ref, Ptr> self;
	typedef __rb_tree_node<T>* link_type;

	__rb_tree_iterator() {}
	__rb_tree_iterator(link_type x) { node = x; }
	__rb_tree_iterator(const iterator& it) { node = it.node; }
	
	reference operator*() const { return link_type(node)->data; } 
	pointer operator->() const { return &(operator*()); }
	self& operator++()
	{
		increment();
		return *this;
	}
	self operator++(int) const
	{
		self tmp = *this;
		increment();
		return tmp;
	}
	self& operator--()
	{
		decrement();
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		decrement();
		return tmp;
	}
};

template<class Key, class T, class KeyOfT, class Compare, class Alloc = alloc>
class rb_tree
{
protected:
	typedef void* void_pointer;
	typedef __rb_tree_node_base* base_ptr;
	typedef __rb_tree_node<T> rb_tree_node;
	typedef simple_alloc<rb_tree_node, alloc> rb_tree_node_allocator;
	typedef __rb_tree_color_type color_type;

public:
	typedef Key key_type;
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef rb_tree_node* link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

protected:
	// 内存分配函数
	link_type get_node()
	{
		return rb_tree_node_allocator::allocate();
	}
	link_type create_node(const value_type& x)
	{
		link_type tmp = get_node();
		construct(&tmp->data, x);
	}
	link_type clone_node(link_type x)
	{
		link_type tmp = create_node(x->data);
		tmp->color = x->color;
		tmp->left = tmp->right = 0;
		return tmp;
	}
	void put_node(link_type p)
	{
		rb_tree_node_allocator::deallocate(p);
	}
	void destroy_node(link_type p)
	{
		destroy(&p->data);
		put_node(p);
	}
	
protected:
	// 唯三成员变量
	size_type node_count;	// 记录树的大小
	link_type header;		// 记录标兵头
	Compare key_compare;	// 记录比较器

	link_type& root() const { return header->parent; }
	link_type& leftmost() { return header->left; }
	link_type& rightmost() { return header->right; }

	// 便利函数
	static link_type& left(link_type x) { return x->left; }
	static link_type& right(link_type x) { return x->right; }
	static link_type& parent(link_type x) { return x->parent; }
	static reference value(link_type x) { return x->data; }
	static const Key& key(link_type x) { return KeyOfT()(x->data); }
	static color_type& color(link_type x) { return x->color; }

	static link_type& left(base_ptr x) { return x->left; }
	static link_type& right(base_ptr x) { return x->right; }
	static link_type& parent(base_ptr x) { return x->parent; }
	static reference value(base_ptr x) { return x->data; }
	static const Key& key(base_ptr x) { return KeyOfT()(x->data); }
	static color_type& color(base_ptr x) { return x->color; }

	static link_type minimum(link_type x) { return __rb_tree_node_base::minimum(x); }
	static link_type maximum(link_type x) { return __rb_tree_node_base::maximum(x); }

public:
	typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
private:
	// 基础隐藏操作
	iterator __insert(base_ptr x, base_ptr y, const value_type& v); 
	link_type __copy(link_type x, link_type p);
	void __erase(link_type x);

	void init()
	{
		header = get_node();
		color(header) = red;
		root() = 0;
		leftmost() = header;
		rightmost() = header;
	}

public:
	// 构造
	rb_tree(const Compare& comp = Compare())
		:node_count(0), key_compare(comp) { init(); }
	~rb_tree() { clear(), put_node(header); }

	rb_tree<Key, T, KeyOfT, Compare, Alloc>&
	operator==(const rb_tree<Key, T, KeyOfT, Compare, Alloc>& x);

public:
	Compare key_comp() const { return key_compare; }
	iterator begin() { return leftmost(); }
	iterator end() { return header; }
	bool empty() const { return node_count == 0; }
	size_type size() const { return node_count; }
	size_type max_size() const { return size_type(-1); }

	pair<iterator, bool> insert_unique(const value_type& x); // 补充pair
	iterator insert_equal(const value_type& x);
};

