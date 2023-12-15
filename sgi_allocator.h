#ifndef __SGI_ALLOCATOR_H__
#define __SGI_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>

#include <new>
#include <type_traits>

#include "sgi_construct.h"

#if 0
#include <new>
#define THROW_BAD_ALLOC throw std::bad_alloc
#elif !defined(THROW_BAD_ALLOC)
#include <iostream>
#define THROW_BAD_ALLOC                        \
    std::cerr << "out of memory" << std::endl; \
    exit(1);
#endif

namespace ZMJ
{

    /*
     *  level one allocator
     */
    using malloc_handler = void (*)();

    template <int inst>
    class malloc_alloc_template
    {
    private:
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static void (*malloc_alloc_oom_handler)();

    public:
        static void *allocate(size_t n)
        {
            void *resutl = malloc(n);
            if (result == 0)
                result = oom_malloc(n);
            return result;
        }

        static void *deallocate(void *p, size_t)
        {
            free(p);
        }

        static reallocate(void *p, size_t old_sz, size_t new_sz)
        {
            void *result = realloc(p, new_sz);
            if (result == 0)
                result = oom_realloc(p, new_sz);
            return result;
        }

        static malloc_handler set_malloc_handler(malloc_handler handler)
        {
            malloc_handler old = malloc_alloc_oom_handler;
            malloc_alloc_oom_handler = handler;
            return old;
        }
    }; // end malloc_alloc_template

    template <int inst>
    void (*malloc_alloc_template<inst>::malloc_alloc_oom_handler)() = 0;

    template <int inst>
    void *malloc_alloc_template<inst>::oom_malloc(size_t n)
    {
        malloc_handler handler;
        void *result;

        for (;;)
        {
            handler = malloc_alloc_oom_handler;
            if (handler == 0)
                THROW_BAD_ALLOC;
            (*handler)();
            result = malloc(n);
            if (result)
                return result;
        }
    }

    template <int inst>
    void *malloc_alloc_template<inst>::oom_realloc(void *old, size_t n)
    {
        malloc_handler handler;
        void *result;
        for (;;)
        {
            handler = malloc_alloc_oom_handler;
            if (handler == 0)
                THROW_BAD_ALLOC;
            (*handler)();
            result = realloc(old, n);
            if (result)
                return result;
        }
    }

    using malloc_alloc = malloc_alloc_template<0>;

    /*
     *   default_allocate_template
     */
    template <bool threads, int inst>
    class default_alloc_template
    {
    private:
        enum
        {
            align = 8
        };
        enum
        {
            max_size = 128
        };
        enum
        {
            nfreelists = max_size / align
        };

    private:
        static size_t round_up(size_t bytes)
        {
            return (bytes + align - 1) & ~(align - 1);
        }

    private:
        union obj
        {
            union obj *free_list_next;
            char client_data[1];
        };

    private:
        static obj *volatile free_list[nfreelists];
        static size_t free_list_index(size_t bytes)
        {
            return (bytes + align - 1) / align - 1;
        }

        static void *refill(size_t n);
        static char *chunk_alloc(size_t size, int &nobjs);

    private:
        static char *start_free;
        static char *end_free;
        static size_t heap_size;

    public:
        static void *allocate(size_t bytes);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
    }; // end default_alloc_template

    template <bool threads, int inst>
    char *default_alloc_template<threads, inst>::start_free = 0;

    template <bool threads, int inst>
    char *default_alloc_template<threads, inst>::end_free = 0;

    template <bool threads, int inst>
    size_t default_alloc_template<threads, inst>::heap_size = 0;

    template <bool threads, int inst>
    default_alloc_template<threads, inst>::obj *volatile default_alloc_template<threads, inst>::free_list[nfreelists] = {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
    }; // end default_alloc_template

    template <bool threads, int inst>
    void *default_alloc_template<threads, inst>::allocate(size_t bytes)
    {
        obj *volatile *free_list_node;
        obj *result;

        if (bytes > (size_t)max_bytes)
        {
            return malloc_alloc::allocate(bytes);
        }

        free_list_node = free_list + free_list_index(bytes);
        result = *free_list_node;

        if (result)
        {
            void *ret = refill(round_up(bytes));
            return ret;
        }

        *free_list_node = result->free_list_next;
        return result;
    }

    template <bool threads, int inst>
    void default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
    {
        obj *q = (obj *)p;
        obj *volatile *free_list_node;

        /*如果大于max_bytes,128字节使用1级分配器*/
        if (n > (size_t)max_bytes)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }
        free_list_node = free_list + free_list_index(n);
        q->free_list_next = (*free_list_node);
        *free_list_node = q;
    }

    template <bool threads, int inst>
    void *default_alloc_template<threads, inst>::reallocate(void *p, size_t old_sz, size_t new_sz)
    {
        obj *volatile *free_list_node;
        obj *q = (obj *)p;
        void *result;

        if (new_sz > (size_t)max_bytes)
        {
            return malloc_alloc::reallocate(p, old_sz, new_sz);
        }

        if (old_sz > new_sz)
            return p;

        result = allocate(new_sz);
        if (result)
        {
            memcpy(result, p, old_sz);
            deallocate(p, old_sz);
        }
        return result;
    }

    template <bool threads, int inst>
    void *default_alloc_template<threads, inst>::refill(size_t n)
    {
        int nodjs = 20; // 默认分配 20块 大小为n的块。
        char *chunk = chunk_alloc(n, nodjs);
        obj *volatile *free_list_node;
        obj *result;
        obj *curr_obj, *next_obj;
        if (nodjs == 1)
            return (chunk);

        free_list_node = free_list + free_list_index(n);

        /*nodjs大于1，预分配了多块，需要把多块用链表串起来*/
        result = (obj *)chunk;
        *free_list_node = next_obj = (obj *)(chunk + n);
        for (int i = 1;; ++i)
        {
            curr_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n);
            if (nodjs - 1 == i)
            {
                curr_obj->next_obj = 0;
                break;
            }
            else
            {
                curr_obj->free_list_next = next_obj;
            }
        }
        return (result);
    }

    template <bool threads, int inst>
    char *default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs)
    {
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        /*内存池余量足够，直接返回*/
        if (bytes_left >= total_bytes)
        {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size;
            total_bytes = (size * nobjs);
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else
        {
            /*
             *   需要分清内存池、free_list是不同的。
             *       内存池：是由start_free,end_free、head_size管理。
             *       free_list：管理从内存池来到空余空间，和用户释放的空间。
             *
             *   重新分配之前，把内存池的剩余空间分配给free_list。
             *   一个原因，要扩容了，需要分配新空间，那么使用realloc，还是malloc呢？
             *       如果是realloc，那么开销就大了，而且也得改改start_free、和end_free，
             *   而且是不可能的，会把free_list管理的内存块全部无效。
             *
             *       那么为什么不把当前start_free ~ end_free的内存全部分配到free_list呢？
             *   这样既不用realloc的内存拷贝也不用负担重新分配之后start_free,end_free应该在哪里的负担。
             *   然后，还可以直接使用malloc重新获得新的内存池。
             */
            /*扩容规则： 2倍需要申请的空间 + 16分之一的当前堆大小 */
            size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
            if (bytes_left > 0)
            {
                /*把内存池的剩余空间分配给free_list*/
                obj *node = free_list + free_list_index(bytes_left);
                *((obj *)(start_free))->free_list_next = *node->free_list_next;
                *node = start_free;
                start_free += bytes_left;
            }

            start_free = (obj *)malloc(bytes_to_get);
            if (start_free == NULL)
            {
                obj *volatile *free_list_node, *p;
                for (int i = size; i <= max_size; i += align)
                {
                    free_list_node = free_list + free_list_index(i);
                    p = *free_list_node;
                    if (p != NULL)
                    {
                        *free_list_node = p->free_list_next;
                        start_free = (obj *)p;
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs);
                    }
                }
                end_free = 0;
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }

#ifdef USE_MALLOC
    typedef malloc_alloc alloc;
#else
    typedef default_alloc_template<false, 0> alloc;
#endif

    template <class T, class Alloc = alloc>
    class simple_alloc
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    public:
        template <class U>
        struct rebind
        {
            using other = simple_alloc<T, U>;
        };

    public:
        static pointer allocate(size_t n)
        {
            return 0 == n ? 0 : (T *)Alloc::allocate(n * sizeof(T));
        }

        static pointer allocate()
        {
            return (T *)Alloc::allocate(sizeof(T));
        }

        static void deallocate(T *p, size_t n)
        {
            if (n != 0)
                Alloc::deallocate(p, n * sizeof(T));
        }

        static void deallocate(T *p)
        {
            Alloc::deallocate(p, sizeof(T));
        }

        static void construct(T *p, const T &val)
        {
            ZMJ::construct(p, val);
        }

        static void destroy(T *p)
        {
            ZMJ::destory(p);
        }

        pointer address(reference x)
        {
            return (pointer)&x;
        }

        const_pointer address(const_reference x)
        {
            return (const_pointer)&x;
        }

        size_type max_size()
        {
            return size_type(UINT_MAX / sizeof(value_type));
        }

    }; // end simple_alloc
} // namespace ZMJ
#endif // sgi_allocator.h