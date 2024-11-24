#pragma once

/*

	<第一级配置器>
	整体逻辑是直接调用原生c接口开辟释放内存,
	申请失败就触发内存不足时的专用函数, 
	要求客户传入处理函数用来释放内存, 在上述专用函数中发挥作用.

*/

#include <iostream>
#include <functional>
#define __THORW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)

class __malloc_alloc_template
{
private:
	// 内存不足时的处理函数
	static void* oom_malloc(size_t);							// 使用malloc操作时内存不足触发
	static void* oom_remalloc(void*, size_t);					// 使用realloc操作时内存不足触发
	static std::function<void()> __malloc_alloc_oom_handler;	// 系统在内存配置需求无法被满足时调用的函数

public:
	static void* allocate(size_t n);												// 使用c原生malloc
	static void deallocate(void* p, size_t n);										// 使用c原生free
	static void* reallocate(void* p, size_t old_size, size_t new_size);				// 使用c原生realloc
	static std::function<void()> set_malloc_handler(std::function<void()> f);		// 用于设置内存不足时的处理例程
};

typedef __malloc_alloc_template malloc_alloc;

typedef __malloc_alloc_template mat;

void* mat::allocate(size_t n)
{
	void* ret = malloc(n);
	if (0 == ret) ret = oom_malloc(n); // 申请失败说明内存不足, 调用特殊版本
	return ret;
}

void mat::deallocate(void* p, size_t n)
{
	free(p);
}

void* mat::reallocate(void* p, size_t old_size, size_t new_size)
{
	void* ret = realloc(p, new_size);
	if (0 == ret) ret = oom_remalloc(p, new_size);
	return ret;
}

// 该函数在内存不足时触发, 用于尝试释放内存
// 该函数调用默认为0, 代表无处理函数, 如果内存不足时其为0, 一般会触发 __THORW_BAD_ALLOC
std::function<void()> mat::__malloc_alloc_oom_handler = 0;

// 用于设置内存不足时的处理函数, 要求客户传入一个类型为function<void()>的参数用于函数调用
// 在更新处理函数的同时, 该函数也会记录旧的处理函数并返回, 方便之后恢复旧状态(好像也没什么用)
std::function<void()> mat::set_malloc_handler(std::function<void()> f)
{
	std::function<void()> old = __malloc_alloc_oom_handler;
	__malloc_alloc_oom_handler = f;
	return old;
}

// 整体逻辑就是循环执行处理函数并申请内存, 直到申请成功
void* mat::oom_malloc(size_t n)
{
	std::function<void()> my_malloc_handler;
	void* ret;

	while (true)
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler)
		{
			__THORW_BAD_ALLOC; // 如果没有设置处理函数直接触发 __THORW_BAD_ALLOC
		}

		my_malloc_handler(); // 调用处理例程, 企图释放内存

		ret = malloc(n);
		if (ret) return ret;
	}
}

void* mat::oom_remalloc(void* p, size_t n)
{
	std::function<void()> my_malloc_handler;
	void* ret;

	while (true)
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler)
		{
			__THORW_BAD_ALLOC;
		}

		my_malloc_handler(); // 调用处理例程, 企图释放内存

		ret = realloc(p, n);
		if (ret) return ret;
	}
}