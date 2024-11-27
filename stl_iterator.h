#pragma once
/*
	�ṩ���ֵ���������
	�ṩ��ȡ����ȡ����ͱ�
	�ṩһ�ֵ������ƶ�����, һ�ֵ���������ⶨ����
*/

#include <cstddef>

// ���ֵ��������Ͷ�Ӧ�ı�ǩ��
	// ΪʲôҪ���ñ�ǩ��? Ϊ�����ú������ػ���ʹ�������ڱ���׶ξͿ���ȷ��ʹ�õĺ���, ������Ч��
struct input_iterator_tag {};													// ˳��ֻ��������
struct output_iterator_tag {};													// ˳��ֻд������
struct forward_iterator_tag : public input_iterator_tag {}; 					// ˳���д������
struct bidirectional_iterator_tag : public forward_iterator_tag {};				// ˫���д������
struct random_access_iterator_tag : public bidirectional_iterator_tag {};		// �����д������


// iterator�� �궨����������Ӧ�ͱ�(�������Ϊ����������)
// ΪʲôҪ����Ӧ�ͱ� ? ��Ϊ�����㷨�г������õ���Щ�ͱ�, ���е������������Ӧ���Լ���ʾ������Щ�ͱ�
template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
struct iterator
{
	typedef Category	iterator_category;		// ����������
	typedef T			value_type;				// ��������ָ֮�������
	typedef Distance	difference_type;		// ��������������
	typedef Pointer		pointer;				// ��������ָ֮���ָ��
	typedef Reference	reference;				// ��������ָ֮�������
};

// traits ��ȡ��
// ΪʲôҪ����ȡ��? Ϊ�˱���ʹ��, �㷨����Ľӿڳ���ֻ�е�������һЩ�������ֵ
// �㷨һ��ʼ���޷�֪����Щ�ͱ��, ����Ҫ���ֶο��Խ���ͨ���������Ϳ�����֪��������ͱ�, �����ֶξ�����ȡ��

template <class Iterator>
struct iterator_traits
{
	typedef typename Iterator::iterator_category	iterator_category;
	typedef typename Iterator::value_type			value_type;
	typedef typename Iterator::difference_type		difference_type;
	typedef typename Iterator::pointer				pointer;
	typedef typename Iterator::reference			reference;
};

// ��ȡ������ԭ��ָ����Ƶ�ƫ�ػ���
template <class T>
struct iterator_traits<T*>
{
	typedef typename random_access_iterator_tag		iterator_category;
	typedef typename T								value_type;
	typedef typename ptrdiff_t						difference_type;
	typedef typename T* pointer;
	typedef typename T& reference;
};

// ��ȡ������pointer to const��Ƶ�ƫ�ػ���
template <class T>
struct iterator_traits<const T*>
{
	typedef typename random_access_iterator_tag		iterator_category;
	typedef typename T								value_type;
	typedef typename ptrdiff_t						difference_type;
	typedef typename const T* pointer;
	typedef typename const T& reference;
};

// ֱ�����õ���������ȡ��ȡ�����������͵ķ��ͺ���
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category	// ��������
iterator_category(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();
}

// ���ھ���ĳ���������� distance_type
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
}

// ���ھ���ĳ���������� value_type
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
}

// advance���� (�����㷨�ڲ������ƶ�)
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

// distance���� (���ڼ�������С, ����size(), count())
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
	// ʹ������Ƕ�����ͱ���ʹ��typename
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
