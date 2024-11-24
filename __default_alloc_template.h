#pragma once

/*
	<第二级配置器>
	该配置器仅接收 __MAX_BYTES 以下的需求, 高于该要求的一律交给一级配置器.
	主要维护一个 freelist 和 一块线性内存 作为内存池.
	整体逻辑是 : allocate -> freelist找有没有剩余的对应内存块, 有就直接取, 没有就申请refill, 把需求内存大小对应的链表填满
				 refill -> 直接向chunk_alloc申请一大块内存, 默认是20块需求大小的内存, 
						   根据返回的iobjs, 为1直接返回, 不为1, 取出1, 剩下的全部头插到对应链表去
				 chunk_alloc -> 维护一段线性内存, 线性内存满足默认需求, 直接返回; 只能满足一个以上, 修改iobjs, 能返回多少就多少;
								一个都不够, 先看剩下的内存可不可以插入freelist中, 在调用malloc, 申请失败与成功都有对应的处理策略
*/


#include "__malloc_alloc_template.h"

enum {__Align = 8};									// 小块最小值
enum {__MAX_BYTES = 128};							// 小块上界
enum {__NumOfFreeList = __MAX_BYTES / __Align};		// 邻接表表头个数

class __default_alloc_template
{
private:
	static size_t round_up(size_t bytes);			// 将bytes上调至__Align的倍数

private:
	// next 和 data 共用同一块内存, 两者可以不同时生效, next仅在freelist中才有其意义, 交到上层就是只是data
	// 一个obj的大小由allocate时传入的n决定
	union obj { 
		union obj* next;
		char client_data[0];							// 柔性数组, 动态扩展大小
	};

private:
	static obj* volatile free_list[__NumOfFreeList];	// 这个数组存的是obj指针, 且每个obj都是一个链表头
	static size_t GetIndex(size_t bytes);			// 根据大小决定在那个list中

public:
	static void* allocate(size_t n);
	static void deallocate(void* p, size_t n);
	static void* reallocate(void* p, size_t old_size, size_t new_size);

private:

	// 返回一个大小为n的对象, 并可能加入大小为n的其他区块到freelist
	static void* refill(size_t n);

	// 配置一块大空间, 大小是nobjs个大小为size的区块
	// nobjs是输出型参数, 默认nobjs是20个, 根据内存情况返回不一样的nobjs
	// 为什么返回值是char* 因为char最小, 就是1字节, 不用考虑实际对象的大小
	static char* chunk_alloc(size_t size, int& nobjs);

	// 内存池状态描述
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
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; //16个空指针

// 将bytes上调至__Align的倍数
size_t dat::round_up(size_t bytes)
{
	return ((bytes + __Align - 1) & ~(__Align - 1));
}

// 根据大小决定去那个list
size_t dat::GetIndex(size_t bytes) // 这里不要求bytes是8的倍数, 只要取出的区块大于等于它就行
{
	// (bytes + __Align - 1)是因为bytes最小是1, 加上__Align - 1后整除__Align一定在[1, 16]的范围内, 而不会到0
	return ((bytes + __Align - 1) / __Align - 1);
}

void* dat::allocate(size_t n)
{
	obj* volatile* ListHeadPointer;  // volatile* 表示 ListHeadPointer 是一个指向 volatile 指针 的指针
	obj* ret;

	// 大于128字节就调用第一级配置器
	if (n > __MAX_BYTES) return malloc_alloc::allocate(n);

	// 分析选用那个链表头指针, 取出指向该指针的指针
	ListHeadPointer = dat::free_list + dat::GetIndex(n);

	// 确定返回的内存指针
	ret = *ListHeadPointer; // 取出指向链表头的指针
	if (!ret)
	{
		// 如果没有可用的内存, 就申请一块大小为round_up(n)的内存
		// 用round_up是因为规定只有16种大小为8的倍数的内存大小, 不能请求其他大小
		void* r = refill(dat::round_up(n));
		return r;
	}

	// 链表头删
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

	// 链表尾插
	obj* volatile* ListHeadPointer;
	ListHeadPointer = dat::free_list + dat::GetIndex(n);
	static_cast<obj*>(p)->next = *ListHeadPointer;
	*ListHeadPointer = static_cast<obj*>(p);
	return;
}

void* dat::refill(size_t n)
{
	// 内部调用chunk_alloc, 通过内存池获取内存
	// 返回值是
	int nobjs = 20; // 默认调用20个
	char* chunk = chunk_alloc(n, nobjs);

	// 根据获取的内存块个数进行不同的操作
	if (1 == nobjs) return chunk; // 只有一个直接返回

	// 有多个留下一个, 其他全部做成链表交给freelist
	obj* volatile* ListHeadPointer;
	obj* ret = nullptr;
	obj* current_obj = nullptr;
	obj* next_obj = nullptr;

	ListHeadPointer = dat::free_list + dat::GetIndex(n);

	ret = (obj*)chunk; // 第一个返回
	*ListHeadPointer = next_obj = (obj*)(chunk + n); // 新链表从第二个开始

	for (int i = 1; i <= nobjs - 1; i++)
	{
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n);
		if (i == nobjs - 1) current_obj->next = nullptr;
		else current_obj->next = next_obj;
	}

	return ret;
}

// 内存池核心, 维护freelist和一段线性内存, 同时记录统共申请的堆内存大小
char* dat::chunk_alloc(size_t size, int& nobjs)
{
	char* ret;
	size_t total_bytes = size * nobjs;				// 初始需求字节数
	size_t bytes_left = end_free - start_free;		// 线性内存的剩余大小

	if (bytes_left >= total_bytes) // 线性内存已经满足需求
	{
		ret = start_free;
		start_free += total_bytes;

		// 从这里返回的nobjs依旧是默认的20
		return ret;
	}
	else if (bytes_left >= size) // 线性内存大于一个区块需求, 就降低要求能给多少给多少
	{
		nobjs = bytes_left / size;  // 整个函数只有这里会修改nobjs, else情况靠递归到这里修改
		total_bytes = size * nobjs;
		ret = start_free;
		start_free += total_bytes;

		// 从这里返回的nobjs降低了要求
		return ret;
	}
	else  // 连一个区块需求都无法满足, 就直接进入核心逻辑, 申请大块线性内存, 维护freelist
	{
		// 在申请之前先看看当前线性内存残余是否可以插入freelist, 因为之后申请完后就不再使用这块内存了, 直到程序结束才会释放
		// 要尽力减少内存碎片
		if (bytes_left > 0)
		{
			// 此处存在一定隐患, 允许在bytes_left小于链表管理的块大小的情况下插入对应链表
			// 但是为了追求最高效率, STL允许这种风险存在
			obj* volatile* ListHeadPointer = dat::free_list + dat::GetIndex(bytes_left);

			((obj*)start_free)->next = *ListHeadPointer;
			*ListHeadPointer = (obj*)start_free;
		}

		// 从堆中申请空间, 补充内存池
		size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4); // 申请的大块内存大小是 初始需求二倍 + 随申请次数增加而越来越大的附加量
		start_free = (char*)malloc(bytes_to_get);

		// 申请失败的处理策略
		if (nullptr == start_free)
		{
			// 在free_list中搜寻有没有比size大的内存块, 找到一个也可以
			obj* volatile* ListHeadPointer;
			for (int i = size; i <= __MAX_BYTES; i += __Align)
			{
				ListHeadPointer = dat::free_list + dat::GetIndex(i);
				// 只要有一个链表有剩余, 就取出一块当作线性内存返回
				if (0 != *ListHeadPointer)
				{
					start_free = (char*)(*ListHeadPointer);
					end_free = start_free + i;
					*ListHeadPointer = (*ListHeadPointer)->next;
					// 从这里返回有可能触发"线性内存已经满足需求", 也可能触发"返回一个以上区块大小空间", 不可能触发最后的else
					return (chunk_alloc(size, nobjs));  // 节省代码用
				}
			}

			// 到最后山穷水尽什么都没有了, 再尝试调用一级配置器, 看看能不能用oom机制挽回
			end_free = 0;
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);
		}

		// 申请成功后更新线性内存变量
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;

		// 递归调用chunk_alloc, 其实就是为了节省代码, 因为申请成功后一定只会触发"线性内存已经满足需求"的情况
		return (chunk_alloc(size, nobjs));
	}

}