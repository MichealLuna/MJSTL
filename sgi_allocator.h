#ifndef __SGI_ALLOCATOR_H__
#define __SGI_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>

#include <new>
#include "type_traits.h"

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

namespace mjstl
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
            void *result = malloc(n);
            if (result == 0)
                result = oom_malloc(n);
            return result;
        }

        static void deallocate(void *p, size_t)
        {
            free(p);
        }

        static void* reallocate(void *p, size_t old_sz, size_t new_sz)
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
    typename default_alloc_template<threads, inst>::obj *volatile default_alloc_template<threads, inst>::free_list[nfreelists] = {
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

        /*如果申请的空间大于128，则使用一级内存分配器。*/
        if (bytes > (size_t)max_size)
        {
            return malloc_alloc::allocate(bytes);
        }

        /*申请的内存空间小于等于128字节，使用二级内存分配器。*/
        /*
        *   考虑多线程，这里需要加一把锁，但这里加锁临界区似乎太大了。
        */
        free_list_node = free_list + free_list_index(bytes);
        result = *free_list_node;

        if (result == 0)
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
        if (n > (size_t)max_size)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        /*如果释放的空间小于等于128字节，说明是使用二级分配器获得的内存。*/
        /*考虑多线程的话，这里也需要加锁限制。*/
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

        if (new_sz > (size_t)max_size)
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
                curr_obj->free_list_next = 0;
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
        }/*内存池余量最少够1个块大小，直接把剩余块都尽量返回。*/
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size;
            total_bytes = (size * nobjs);
            result = start_free;
            start_free += total_bytes;
            return result;
        }/*内存池余量连1个size大小的块都没了，必须申请。*/
        else
        {
            /*
             *   需要分清内存池和free_list是不同的。
             *       内存池：是由start_free,end_free、head_size管理。
             *       free_list：管理从内存池获得的空间，并管理空余空间。
             *
             *   重新分配之前，把内存池的剩余空间分配给free_list。
             *   一个原因，要扩容了，需要分配新空间，那么使用realloc，还是malloc呢？
             *       如果是realloc，那么开销就大了，而且也得改改start_free、和end_free，
             *   并且realloc会把free_list管理的内存块全部无效。
             *
             *       那么为什么不把当前start_free ~ end_free的内存全部分配到free_list呢？
             *   这样既不用realloc的内存拷贝也不用考虑重新分配之后start_free,end_free应该在哪里。
             *   然后，还可以直接使用malloc重新获得新的内存池。
             */
            /*扩容规则： 2倍需要申请的空间 + 16分之一的当前堆大小 */
            size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
            if (bytes_left > 0)
            {
                /*把内存池的剩余空间分配给free_list*/
                obj * volatile *node = free_list + free_list_index(bytes_left);
                ((obj *)(start_free))->free_list_next = *node;
                *node = (obj*)start_free;
            }

            start_free = (char *)malloc(bytes_to_get);
            /*如果malloc还是没有分配到内存，那么就看看free_list里面的空闲空间。*/
            if (start_free == NULL)
            {
                obj *volatile *free_list_node, *p;
                for (int i = size; i <= max_size; i += align)
                {
                    free_list_node = free_list + free_list_index(i);
                    p = *free_list_node;
                    /*在空闲空间找到。*/
                    if (p != NULL)
                    {
                        /*这里需要从空闲链表中取出一个块，给内存池扩充。*/
                        *free_list_node = p->free_list_next;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        /*
                        *   递归，但不用担心，其实只有1层。
                        * 因为这里的start_free,end_free已经获得了足够的空间，
                        * 进入下一层后，一定可以获得至少一个size大小的块，要么
                        * 进入第一个if，要么第二个else if，不会来到这里。
                        */
                        return chunk_alloc(size, nobjs);
                    }
                    /*如果没有，则向下一个8的倍数空间获取。*/
                }
                /*      如果空闲空间也没有。
                *   那么调用一级空间配置器，其实也不指望它可以获得，
                * 主要目的是通过这个配置器，触发异常，通知外界。
                */
                end_free = 0;
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }

            /*      malloc直接获得足够的内存。
            *   更新内存池空间，并重新调用chunk_alloc，值得注意的是，
            *   如果malloc没有分配到足够的内存进入上面的if代码块，
            *   它不会执行到下面的代码块，因为在if块内要么返回，要么异常。
            */
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

    template <class T,class Alloc = alloc>
    class simple_alloc
    {
    public:
        typedef T               value_type;
        typedef T*                pointer;
        typedef const T*          const_pointer;
        typedef T&                reference;
        typedef const T&          const_reference;
        typedef size_t            size_type;
        typedef ptrdiff_t         difference_type;

    public:
        template<class U>
        simple_alloc(const simple_alloc<U>&) noexcept{}
        
        simple_alloc(){}

        malloc_handler set_oom_handler(malloc_handler hdl){
            malloc_handler old = malloc_alloc_template<0>::malloc_alloc_oom_handler;
            malloc_alloc_template<0>::malloc_alloc_oom_handler = hdl;
            return old;
        }
    public:
        template <class U>
        struct rebind
        {
            using other = simple_alloc<U>;
        };

    public:
        pointer allocate(size_t n);
        pointer allocate();
        void deallocate(T *p, size_t n);
        void deallocate(T *p);
        void construct(T* ptr);
        void construct(T *p, const T &val);
        void destroy(T *p);
        void destroy(T* first,T* last);
        pointer address(reference x);
        const_pointer address(const_reference x);
        size_type max_size();
    }; // end simple_alloc


template<class T,class Alloc>
typename simple_alloc<T,Alloc>::pointer 
simple_alloc<T,Alloc>::allocate(size_t n)
{
    return 0 == n ? 0 : (T *)alloc::allocate(n * sizeof(T));
}

template<class T,class Alloc>
typename simple_alloc<T,Alloc>::pointer 
simple_alloc<T,Alloc>::allocate()
{
    return (T *)alloc::allocate(sizeof(T));
}

template<class T,class Alloc>
void simple_alloc<T,Alloc>::deallocate(T *p, size_t n)
{
    if (p == 0) return;
    alloc::deallocate(p, n * sizeof(T));
}

template<class T,class Alloc>
void simple_alloc<T,Alloc>::deallocate(T *p)
{
    if(p == 0) return;
    alloc::deallocate(p, sizeof(T));
}

template<class T,class Alloc>
void simple_alloc<T,Alloc>::construct(T* ptr){
    mjstl::construct(ptr);
}

template<class T,class Alloc>
void simple_alloc<T,Alloc>::construct(T *p, const T &val)
{
    mjstl::construct(p, val);
}

template<class T,class Alloc>
void simple_alloc<T,Alloc>::destroy(T *p)
{
    mjstl::destory(p);
}

template<class T,class Alloc>
void simple_alloc<T,Alloc>::destroy(T* first,T* last){
    mjstl::destory(first,last);
}

template<class T,class Alloc>
typename simple_alloc<T,Alloc>::pointer 
simple_alloc<T,Alloc>::address(reference x)
{
    return (pointer)&x;
}

template<class T,class Alloc>
typename simple_alloc<T,Alloc>::const_pointer 
simple_alloc<T,Alloc>::address(const_reference x)
{
    return (const_pointer)&x;
}

template<class T,class Alloc>
typename simple_alloc<T,Alloc>::size_type 
simple_alloc<T,Alloc>::max_size()
{
    return size_type(UINT_MAX / sizeof(value_type));
}

} // namespace ZMJ
#endif // !__SGI_ALLOCATOR_H__