#pragma once
#include "stl_iterator.h"

/*
	����ȡ���Ļ�����, ������ȡ����value_type��һ����ȡ�����͵�����
	���ڱ��POD���ͼ�һЩ�Զ����ɲ���Ĭ�Ϻ�������
	���ڵײ���ݲ�ͬ����ʵ�ֲ�ͬ�Ĳ���
*/

// __type_traits�� ������Բ�ͬ���ͱ�����, �ڱ���ʱ����ɺ������;���
// ����˵�����ڲ�������һЩ�ͱ�����, �����Ƿ��Զ����ɿ���/����/��������/��ֵ����, �Ƿ�Ϊ��ȫƽ����
// �Ƿ�ƽ�������Ż᲻����䶯̬�ڴ�, ��û�ж�̬�ȸ��ӻ���, 
// һ��ƽ�������ֱ����memcpy/memmoveֱ���ƶ��ײ�, �ǳ�����

// ����ʹ�÷��� : ��һ�� : ��ȡ�����value_type, ������һ��
//				  �ڶ��� : value_type����__type_traits, ������ȡ��ϣ������������, ������һ������, 
//				  ������ : ���ú�������, �����trueѡ���Ч����, �����falseѡ��ȫ����

struct __true_type {};
struct __false_type {};

template<class type>
struct __type_traits
{
	typedef __true_type this_dummy_member_must_be_first;

	typedef __false_type  has_trivial_default_constructor;
	typedef __false_type  has_trivial_copy_constructor;
	typedef __false_type  has_trivial_destructor;
	typedef __false_type  has_trivial_assignment_operator;
	typedef __false_type  is_POD_type;
};

// ������C++Ĭ���ͱ���ȫ�ػ��汾
template <>
struct __type_traits<char>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<signed char>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned char>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<short>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned short>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<int>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned int>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<long>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned long>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<float>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<double>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<long double>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <class T>
struct __type_traits<T*>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};