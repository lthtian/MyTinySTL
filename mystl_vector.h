#pragma once
#include "simple_alloc.h"
#include "stl_construct.h"
#include "stl_uninitialized.h"

namespace lth
{
	template<class T, class Alloc = alloc>
	class vector
	{
	public:
		// vector����������õ�����, ��Ϊ��ά�������ռ�, ֱ����ָ��Ϳ���ʵ�����е�����������Ϊ, ������ra��
		typedef T* iterator;

		// ����������ͱ�
		typedef T			value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

	protected:
		// ����һ���ռ�������
		typedef simple_alloc<value_type, Alloc> data_allocator;
		// �����������������ĵ�����, Ψ����Ҫά���ĳ�Ա����
		iterator start, finish, end_of_storage;

		// Ĭ������sta -> end_of_storage
		void deallocate()
		{
			if (start) data_allocator::deallocate(start, end_of_storage - start);
		}

		// ��ʼ�����value + �趨����������
		void fill_initialize(size_type n, const T& value)
		{
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}


		// ����fill_initalize�еĳ�ʼ����䲿��
		iterator allocate_and_fill(size_type n, const T& x)
		{
			iterator ret = data_allocator::allocate(n);
			uninitialized_fill_n(ret, n, x);
			return ret;
		}

		void insert_aux(iterator position, const T& x);		// ����ǰ����, ֻ��push_back����

	public:
		// ��������ʵ��
		iterator begin() { return start; }
		iterator end() { return finish; }
		iterator begin() const { return start; }
		iterator end() const { return finish; }
		size_type size() const { return size_type(end() - begin()); }
		size_type capacity() const { return size_type(end_of_storage - begin()); }
		bool empty() const { return begin() == end(); }
		reference operator[](size_type n) { return *(begin() + n); }

		//Ĭ�ϳ�Ա��������
		vector() :start(0), finish(0), end_of_storage(0) {}
		vector(size_type n, const T& value) { fill_initialize(n, value); }
		vector(int n, const T& value) { fill_initialize(n, value); }
		vector(long n, const T& value) { fill_initialize(n, value); }
		explicit vector(size_type n) { fill_initialize(n, T()); }
		~vector() { destroy(start, finish); deallocate(); }


		// ר������
		reference front() { return *begin(); }
		reference back() { return *(end() - 1); }
		void push_back(const T& x);
		void pop_back();
		void insert(iterator pos, size_type n, const T& x);
		iterator erase(iterator pos);
		iterator erase(iterator first, iterator last);
		void resize(size_type new_size, const T& x)
		{
			if (new_size < size()) erase(begin() + new_size, end());
			else insert(end(), new_size - size(), x);
		}
		void resize(size_type size) { resize(size, T()); }
		void clear() { erase(begin(), end()); }
	};

	template<class T, class Alloc>
	void vector<T, Alloc>::push_back(const T& x)
	{
		if (finish != end_of_storage)
		{
			construct(finish, x);
			++finish;
		}
		else insert_aux(end(), x);
	}

	// �������
	template<class T, class Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T& x)
	{
		if (finish != end_of_storage) // ��Ȼ��ʣ��ռ�
		{
			construct(finish, *(finish - 1)); // β���ȹ������
			++finish;
			T x_copy = x;
			std::copy_backward(position, finish - 2, finish - 1); // ��[pos, fin - 2) ������ [pos + 1, fin - 1)
			*position = x_copy;
		}
		else	// û��ʣ��ռ�
		{
			// ����һ�οռ�
			int len = size() ? size() * 2 : 1;
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			// ��֤�쳣����ʱ, ���������´���������ڴ�, ��ʵ�Ǳ�֤�쳣��ȫ��
			try
			{
				new_finish = uninitialized_copy(start, position, new_finish);
				construct(new_finish, x);
				++new_finish;
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...)
			{
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			destroy(begin(), end());
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}

	// �������n
	template<class T, class Alloc>
	void vector<T, Alloc>::insert(iterator pos, size_type n, const T& x)
	{
		if (n == 0) return;

		if (size_type(end_of_storage - finish) >= n) // ʣ��ռ���ڵ�������Ԫ�ظ���
		{
			T x_copy = x;
			const size_type PosToFin = finish - pos; // ����pos���ж��ٵ�
			iterator old_finish = finish;

			if (PosToFin >= n)
			{
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				std::copy_backward(pos, old_finish - n, old_finish);
				std::fill(pos, pos + n, x_copy);
			}
			else
			{
				uninitialized_fill_n(finish, n - PosToFin, x_copy);
				finish += n - PosToFin;
				uninitialized_copy(pos, old_finish, finish);
				finish += PosToFin;
				std::fill(pos, old_finish, x_copy);
			}
		}
		else	// ʣ��ռ�С������Ԫ�ظ���
		{
			// �����¿ռ�
			const size_type len = size() + std::max(size(), n);

			// ����Լ����insert_aux�Ĳ���
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			// ��֤�쳣����ʱ, ���������´���������ڴ�, ��ʵ�Ǳ�֤�쳣��ȫ��
			try
			{
				new_finish = uninitialized_copy(start, pos, new_finish);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(pos, finish, new_finish);
			}
			catch (...)
			{
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			destroy(begin(), end());
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::pop_back()
	{
		--finish;
		destroy(finish);
	}

	// erase����ɾ�������ĵ�һ����Ч������
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator pos)
	{
		if (pos + 1 != end()) std::copy(pos + 1, finish, pos);
		--finish;
		destroy(finish);
		return pos;
	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{
		iterator i = std::copy(last, finish, first);
		destroy(i, finish);
		finish = finish - (last - first);
		return first;
	}
}