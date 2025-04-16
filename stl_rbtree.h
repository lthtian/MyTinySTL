#pragma once
#include "simple_alloc.h"
#include "stl_iterator.h"
#include "stl_construct.h"
#include "stl_comparator.h"
#include "stl_uninitialized.h"
#include "stl_util.h"

// �ڵ���ɫ
typedef bool __rb_tree_color_type;
const __rb_tree_color_type red = false;
const __rb_tree_color_type black = true;

// �ڵ�����
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

// �ڲ�ʵ��ָ��ṹ(�͵�������ʵ���ƶ�����), ���������ͺ�����(�͵���������������)
template<class T>
struct __rb_tree_node : public __rb_tree_node_base
{
	typedef __rb_tree_node<T>* link_type; 
	T data;
};


// ������
struct __rb_tree_iterator_base
{
	typedef __rb_tree_node_base::base_ptr base_ptr;
	typedef bidirectional_iterator_tag iterator_category; // ����˫�������
	typedef ptrdiff_t difference_type;

	base_ptr node;  // ��ڵ��Ψһ��Ҫ��ϵ

	// �ײ�������ƶ� -- ������� + header����״���ж�
	void increment()
	{
		if (node->right != 0) // �ұ߻����ӽڵ�, ��������ջ��ƶ����ҵ�����
		{
			node = node->right;
			// һ·����
			while (node->left != nullptr) node = node->left;
		}
		else // �ұ����ӽڵ�, �����Ҹ��ڵ�
		{
			base_ptr fa = node->parent;
			// �����ҵĸ��ڵ㲻�����ҽڵ�, ������ҽڵ�˵���϶��Ѿ���������, ���ϻ���
			while (fa->right == node)
			{
				node = fa;
				fa = node->parent;
			}

			if (node->right != fa) node = fa;
			// ��֪ǰ����root��header��Ϊparent
			// ֻ�����ߵ�endʱ, �Ż�һ·�������ϻ��ݵ�root��header
			// ���Ľ����node��ֵΪheader, fa��ֵΪroot, ���ҽ��д�ʱnode->right = fa
			// ��node->right = faʱ, Ӧ������end(), Ҳ����header, Ҳ����node, ��˲���
			// �� != ʱ, ���������ҷ�, ��һ���ڵ�Ӧ����fa, ����node = fa;
		}
	}

	void decrement()
	{
		// �����������nodeΪheaderʱ, header = end(), ����--Ӧ�������ҽڵ�
		if (node->color == red && node->parent->parent == node) node = node->right;
		else if (node->left != 0) // ������������ľ������
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
	// �ڴ���亯��
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
	// Ψ����Ա����
	size_type node_count;	// ��¼���Ĵ�С
	link_type header;		// ��¼���ͷ
	Compare key_compare;	// ��¼�Ƚ���

	link_type& root() const { return header->parent; }
	link_type& leftmost() { return header->left; }
	link_type& rightmost() { return header->right; }

	// ��������
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
	// �������ز���
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
	// ����
	rb_tree(const Compare& comp = Compare())
		:node_count(0), key_compare(comp) { init(); }
	~rb_tree() { clear(), put_node(header); } // ����clear

	//rb_tree<Key, T, KeyOfT, Compare, Alloc>&
	//operator==(const rb_tree<Key, T, KeyOfT, Compare, Alloc>& x);

public:
	Compare key_comp() const { return key_compare; }
	iterator begin() { return leftmost(); }
	iterator end() { return header; }
	bool empty() const { return node_count == 0; }
	size_type size() const { return node_count; }
	size_type max_size() const { return size_type(-1); }

	pair<iterator, bool> insert_unique(const value_type& value); // �������ظ�����
	iterator insert_equal(const value_type& value);	// �����ظ�����
};



template<class Key, class T, class KeyOfT, class Compare, class Alloc>
typename rb_tree<Key, T, KeyOfT, Compare, Alloc>::iterator
rb_tree<Key, T, KeyOfT, Compare, Alloc>::insert_equal(const value_type& value)
{
	link_type fa = header;
	link_type x = root();

	while (x != 0)
	{
		fa = x;
		x = key_compare()(KeyOfT()(value), key(x)) ? left(x) : right(x);
	}
	return __insert(x, fa, value);
}

template<class Key, class T, class KeyOfT, class Compare, class Alloc>
pair<typename rb_tree<Key, T, KeyOfT, Compare, Alloc>::iterator, bool>
rb_tree<Key, T, KeyOfT, Compare, Alloc>::insert_unique(const value_type& value)
{
	link_type fa = header;
	link_type x = root();

	bool flag = true;
	while (x != 0)
	{
		fa = x;
		flag = key_compare()(KeyOfT()(value), key(x));
		x = flag ? left(x) : right(x);
	}

	iterator j = iterator(fa);
	if (flag)	// ������������
	{
		// ���ڵ�һ���ȸ��ڵ�С
		if (j == begin()) return pair<iterator, bool>(__insert(x, fa, value), true); // ������ڵ�������ڵ�, ˵�����ڵ�һ��С��ү�ڵ�
		else --j; // ������ڵ㲻������ڵ�, ���ڵ���ү�ڵ�����ظ�, ��Ҫ�Ƶ�ү�ڵ��ж��Ƿ��ظ�
	}

	if(key_compare(key(j.node), KeyOfT()(value)))
		return pair<iterator, bool>(__insert(x, fa, value), true);

	return pair<iterator, bool>(j, false);
}


template<class Key, class T, class KeyOfT, class Compare, class Alloc>
typename rb_tree<Key, T, KeyOfT, Compare, Alloc>::iterator
rb_tree<Key, T, KeyOfT, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const value_type& v)
{
	link_type x = (link_type)x_;
	link_type y = (link_type)y_;

	link_type z;

	if (y == header || x != nullptr || key_compare(KeyOfT()(v), key(y)))
	{
		z = create_node(v);
		left(y) = z;

		if (y == header)
		{
			root() = z;
			rightmost() = z;
		}
		else if (y == leftmost())
			leftmost() = z;
	}
	else
	{
		z = create_node(v);
		right(y) = z;
		if (y == rightmost()) rightmost() = z;
	}
	parent(z) = y;
	left(z) = nullptr;
	right(z) = nullptr;

	__rb_tree_rebalance(z, header->parent);
	++node_count;
	return iterator(z);
}


// ����
inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base* root)
{
	__rb_tree_node_base* y = x->right; // yΪ��ת������ӽڵ�, yҪ����x��λ��
	x->right = y->left;
	y->left->parent = x;
	y->parent = x->parent;

	if (x == root) root = y;
	else if (x == x->parent->left) x->parent->left = y;
	else x->parent->right = y;
	
	y->left = x;
	x->parent = y;
}

inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base* root)
{
	__rb_tree_node_base* y = x->left; // yΪ��ת������ӽڵ�, yҪ����x��λ��
	x->left = y->right;
	if (y->right != 0) y->right->parent = x;
	y->parent = x->parent;

	if (x == root) root = y;
	else if (x == x->parent->right) x->parent->right = y;
	else x->parent->left = y;
	y->right = x;
	x->parent = y;
}


// �Ӳ�����������ϼ��, �ж��Ƿ���������ͬʱ�޸���ɫ
inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base* root)
{
	x->color = red; // �½ڵ�غ�
	while (x != root && x->parent->color == red) // �����޸���ɫ
	{
		// �Ҳ���
		if (x->parent == x->parent->parent->left) 
		{
			__rb_tree_node_base* y = x->parent->parent->right;
			if (y && y->color == red) // �Ҳ�����Ϊ��
			{
				x->parent->color = black;
				y->color = black;
				x->parent->parent->color = red;
				x = x->parent->parent;
			}
			else // �޲���, �򲮸�Ϊ�� // ˵���߶Ȳ�������, Ҫ��ת
			{
				if (x == x->parent->right) // �½ڵ�Ϊ���ڵ�����ӽڵ�, ��������˫��
				{
					x = x->parent;
					__rb_tree_rotate_left(x, root);
				}
				x->parent->color = black;
				x->parent->parent->color = red;
				__rb_tree_rotate_right(x->parent->parent, root);
			}
		}
		else  // �����游���ҽڵ�
		{
			__rb_tree_node_base* y = x->parent->parent->left;
			if (y && y->color == red) // �Ҳ�����Ϊ��
			{
				x->parent->color = black;
				y->color = black;
				x->parent->parent->color = red;
				x = x->parent->parent;
			}
			else // �޲���, �򲮸�Ϊ�� // ˵���߶Ȳ�������, Ҫ��ת
			{
				if (x == x->parent->left) // �½ڵ�Ϊ���ڵ�����ӽڵ�, ��������˫��
				{
					x = x->parent;
					__rb_tree_rotate_right(x, root);
				}
				x->parent->color = black;
				x->parent->parent->color = red;
				__rb_tree_rotate_left(x->parent->parent, root);
			}
		}
	}
	root->color = black; // ���ڵ���ԶΪ��
}


