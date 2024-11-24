#pragma once

/*
	<�ڶ���������>
	�������������� __MAX_BYTES ���µ�����, ���ڸ�Ҫ���һ�ɽ���һ��������.
	��Ҫά��һ�� freelist �� һ�������ڴ� ��Ϊ�ڴ��.
	�����߼��� : allocate -> freelist����û��ʣ��Ķ�Ӧ�ڴ��, �о�ֱ��ȡ, û�о�����refill, �������ڴ��С��Ӧ����������
				 refill -> ֱ����chunk_alloc����һ����ڴ�, Ĭ����20�������С���ڴ�, 
						   ���ݷ��ص�iobjs, Ϊ1ֱ�ӷ���, ��Ϊ1, ȡ��1, ʣ�µ�ȫ��ͷ�嵽��Ӧ����ȥ
				 chunk_alloc -> ά��һ�������ڴ�, �����ڴ�����Ĭ������, ֱ�ӷ���; ֻ������һ������, �޸�iobjs, �ܷ��ض��پͶ���;
								һ��������, �ȿ�ʣ�µ��ڴ�ɲ����Բ���freelist��, �ڵ���malloc, ����ʧ����ɹ����ж�Ӧ�Ĵ������
*/


#include "__malloc_alloc_template.h"

enum {__Align = 8};									// С����Сֵ
enum {__MAX_BYTES = 128};							// С���Ͻ�
enum {__NumOfFreeList = __MAX_BYTES / __Align};		// �ڽӱ��ͷ����

class __default_alloc_template
{
private:
	static size_t round_up(size_t bytes);			// ��bytes�ϵ���__Align�ı���

private:
	// next �� data ����ͬһ���ڴ�, ���߿��Բ�ͬʱ��Ч, next����freelist�в���������, �����ϲ����ֻ��data
	// һ��obj�Ĵ�С��allocateʱ�����n����
	union obj { 
		union obj* next;
		char client_data[0];							// ��������, ��̬��չ��С
	};

private:
	static obj* volatile free_list[__NumOfFreeList];	// �����������objָ��, ��ÿ��obj����һ������ͷ
	static size_t GetIndex(size_t bytes);			// ���ݴ�С�������Ǹ�list��

public:
	static void* allocate(size_t n);
	static void deallocate(void* p, size_t n);
	static void* reallocate(void* p, size_t old_size, size_t new_size);

private:

	// ����һ����СΪn�Ķ���, �����ܼ����СΪn���������鵽freelist
	static void* refill(size_t n);

	// ����һ���ռ�, ��С��nobjs����СΪsize������
	// nobjs������Ͳ���, Ĭ��nobjs��20��, �����ڴ�������ز�һ����nobjs
	// Ϊʲô����ֵ��char* ��Ϊchar��С, ����1�ֽ�, ���ÿ���ʵ�ʶ���Ĵ�С
	static char* chunk_alloc(size_t size, int& nobjs);

	// �ڴ��״̬����
	static char* start_free;
	static char* end_free;
	static size_t heap_size;
};


typedef __default_alloc_template dat;

char* dat::start_free = 0;
char* dat::end_free = 0;
size_t dat::heap_size = 0;

dat::obj* volatile dat::free_list[__NumOfFreeList] =
{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; //16����ָ��

// ��bytes�ϵ���__Align�ı���
size_t dat::round_up(size_t bytes)
{
	return ((bytes + __Align - 1) & ~(__Align - 1));
}

// ���ݴ�С����ȥ�Ǹ�list
size_t dat::GetIndex(size_t bytes) // ���ﲻҪ��bytes��8�ı���, ֻҪȡ����������ڵ���������
{
	// (bytes + __Align - 1)����Ϊbytes��С��1, ����__Align - 1������__Alignһ����[1, 16]�ķ�Χ��, �����ᵽ0
	return ((bytes + __Align - 1) / __Align - 1);
}

void* dat::allocate(size_t n)
{
	obj* volatile* ListHeadPointer;  // volatile* ��ʾ ListHeadPointer ��һ��ָ�� volatile ָ�� ��ָ��
	obj* ret;

	// ����128�ֽھ͵��õ�һ��������
	if (n > __MAX_BYTES) return malloc_alloc::allocate(n);

	// ����ѡ���Ǹ�����ͷָ��, ȡ��ָ���ָ���ָ��
	ListHeadPointer = dat::free_list + dat::GetIndex(n);

	// ȷ�����ص��ڴ�ָ��
	ret = *ListHeadPointer; // ȡ��ָ������ͷ��ָ��
	if (!ret)
	{
		// ���û�п��õ��ڴ�, ������һ���СΪround_up(n)���ڴ�
		// ��round_up����Ϊ�涨ֻ��16�ִ�СΪ8�ı������ڴ��С, ��������������С
		void* r = refill(dat::round_up(n));
		return r;
	}

	// ����ͷɾ
	*ListHeadPointer = ret->next;
	return ret;
}

void dat::deallocate(void* p, size_t n)
{
	if (n > __MAX_BYTES)
	{
		malloc_alloc::deallocate(p, n);
		return;
	}

	// ����β��
	obj* volatile* ListHeadPointer;
	ListHeadPointer = dat::free_list + dat::GetIndex(n);
	static_cast<obj*>(p)->next = *ListHeadPointer;
	*ListHeadPointer = static_cast<obj*>(p);
	return;
}

void* dat::refill(size_t n)
{
	// �ڲ�����chunk_alloc, ͨ���ڴ�ػ�ȡ�ڴ�
	// ����ֵ��
	int nobjs = 20; // Ĭ�ϵ���20��
	char* chunk = chunk_alloc(n, nobjs);

	// ���ݻ�ȡ���ڴ��������в�ͬ�Ĳ���
	if (1 == nobjs) return chunk; // ֻ��һ��ֱ�ӷ���

	// �ж������һ��, ����ȫ������������freelist
	obj* volatile* ListHeadPointer;
	obj* ret = nullptr;
	obj* current_obj = nullptr;
	obj* next_obj = nullptr;

	ListHeadPointer = dat::free_list + dat::GetIndex(n);

	ret = (obj*)chunk; // ��һ������
	*ListHeadPointer = next_obj = (obj*)(chunk + n); // ������ӵڶ�����ʼ

	for (int i = 1; i <= nobjs - 1; i++)
	{
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n);
		if (i == nobjs - 1) current_obj->next = nullptr;
		else current_obj->next = next_obj;
	}

	return ret;
}

// �ڴ�غ���, ά��freelist��һ�������ڴ�, ͬʱ��¼ͳ������Ķ��ڴ��С
char* dat::chunk_alloc(size_t size, int& nobjs)
{
	char* ret;
	size_t total_bytes = size * nobjs;				// ��ʼ�����ֽ���
	size_t bytes_left = end_free - start_free;		// �����ڴ��ʣ���С

	if (bytes_left >= total_bytes) // �����ڴ��Ѿ���������
	{
		ret = start_free;
		start_free += total_bytes;

		// �����ﷵ�ص�nobjs������Ĭ�ϵ�20
		return ret;
	}
	else if (bytes_left >= size) // �����ڴ����һ����������, �ͽ���Ҫ���ܸ����ٸ�����
	{
		nobjs = bytes_left / size;  // ��������ֻ��������޸�nobjs, else������ݹ鵽�����޸�
		total_bytes = size * nobjs;
		ret = start_free;
		start_free += total_bytes;

		// �����ﷵ�ص�nobjs������Ҫ��
		return ret;
	}
	else  // ��һ�����������޷�����, ��ֱ�ӽ�������߼�, �����������ڴ�, ά��freelist
	{
		// ������֮ǰ�ȿ�����ǰ�����ڴ�����Ƿ���Բ���freelist, ��Ϊ֮���������Ͳ���ʹ������ڴ���, ֱ����������Ż��ͷ�
		// Ҫ���������ڴ���Ƭ
		if (bytes_left > 0)
		{
			// �˴�����һ������, ������bytes_leftС���������Ŀ��С������²����Ӧ����
			// ����Ϊ��׷�����Ч��, STL�������ַ��մ���
			obj* volatile* ListHeadPointer = dat::free_list + dat::GetIndex(bytes_left);

			((obj*)start_free)->next = *ListHeadPointer;
			*ListHeadPointer = (obj*)start_free;
		}

		// �Ӷ�������ռ�, �����ڴ��
		size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4); // ����Ĵ���ڴ��С�� ��ʼ������� + ������������Ӷ�Խ��Խ��ĸ�����
		start_free = (char*)malloc(bytes_to_get);

		// ����ʧ�ܵĴ������
		if (nullptr == start_free)
		{
			// ��free_list����Ѱ��û�б�size����ڴ��, �ҵ�һ��Ҳ����
			obj* volatile* ListHeadPointer;
			for (int i = size; i <= __MAX_BYTES; i += __Align)
			{
				ListHeadPointer = dat::free_list + dat::GetIndex(i);
				// ֻҪ��һ��������ʣ��, ��ȡ��һ�鵱�������ڴ淵��
				if (0 != *ListHeadPointer)
				{
					start_free = (char*)(*ListHeadPointer);
					end_free = start_free + i;
					*ListHeadPointer = (*ListHeadPointer)->next;
					// �����ﷵ���п��ܴ���"�����ڴ��Ѿ���������", Ҳ���ܴ���"����һ�����������С�ռ�", �����ܴ�������else
					return (chunk_alloc(size, nobjs));  // ��ʡ������
				}
			}

			// �����ɽ��ˮ��ʲô��û����, �ٳ��Ե���һ��������, �����ܲ�����oom�������
			end_free = 0;
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);
		}

		// ����ɹ�����������ڴ����
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;

		// �ݹ����chunk_alloc, ��ʵ����Ϊ�˽�ʡ����, ��Ϊ����ɹ���һ��ֻ�ᴥ��"�����ڴ��Ѿ���������"�����
		return (chunk_alloc(size, nobjs));
	}

}