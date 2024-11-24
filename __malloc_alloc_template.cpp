#include "__malloc_alloc_template.h"

typedef __malloc_alloc_template mat;

void* mat::allocate(size_t n)
{
	void* ret = malloc(n);
	if (0 == ret) ret = oom_malloc(n); // ����ʧ��˵���ڴ治��, ��������汾
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

// �ú������ڴ治��ʱ����, ���ڳ����ͷ��ڴ�
// �ú�������Ĭ��Ϊ0, �����޴�����, ����ڴ治��ʱ��Ϊ0, һ��ᴥ�� __THORW_BAD_ALLOC
std::function<void()> mat::__malloc_alloc_oom_handler = 0;

// ���������ڴ治��ʱ�Ĵ�����, Ҫ��ͻ�����һ������Ϊfunction<void()>�Ĳ������ں�������
// �ڸ��´�������ͬʱ, �ú���Ҳ���¼�ɵĴ�����������, ����֮��ָ���״̬(����Ҳûʲô��)
std::function<void()> mat::set_malloc_handler(std::function<void()> f)
{
	std::function<void()> old = __malloc_alloc_oom_handler;
	__malloc_alloc_oom_handler = f;
	return old;
}

// �����߼�����ѭ��ִ�д������������ڴ�, ֱ������ɹ�
void* mat::oom_malloc(size_t n)
{
	std::function<void()> my_malloc_handler;
	void* ret;
	
	while (true)
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler)
		{
			__THORW_BAD_ALLOC; // ���û�����ô�����ֱ�Ӵ��� __THORW_BAD_ALLOC
		}

		my_malloc_handler(); // ���ô�������, ��ͼ�ͷ��ڴ�

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

		my_malloc_handler(); // ���ô�������, ��ͼ�ͷ��ڴ�

		ret = realloc(p, n);
		if (ret) return ret;
	}
}