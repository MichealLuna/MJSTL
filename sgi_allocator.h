#ifndef __SGI_ALLOCATOR_H__
#define __SGI_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>

#include <new>
#include<type_traits>

#if 0
    #include <new>
    #define THROW_BAD_ALLOC throw std::bad_alloc
#elif !defined(THROW_BAD_ALLOC)
    #include <iostream>
    #define THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1);
#endif

namespace ZMJ
{

    /*
     *  level one allocator
     */
    using malloc_handler = void (*)();

    template <int inst>
    class malloc_alloc_template{
    private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void*,size_t);
    static void (* malloc_alloc_oom_handler)();

    public:
        static void* allocate(size_t n){
            void* resutl = malloc(n);
            if(result == 0)
                result = oom_malloc(n);
            return result;
        }

        static void* deallocate(void* p,size_t){
            free(p);
        }

        static reallocate(void* p,size_t old_sz,size_t new_sz){
            void* result = realloc(p,new_sz);
            if(result == 0)
                result = oom_realloc(p,new_sz);
            return result;
        }

        
        static malloc_handler set_malloc_handler(malloc_handler handler){
            malloc_handler old = malloc_alloc_oom_handler;
            malloc_alloc_oom_handler = handler;
            return old;
        }
    };
    template<int inst>
    void (*malloc_alloc_template<inst>::malloc_alloc_oom_handler)() = 0;

    template<int inst>
    void* malloc_alloc_template<inst>::oom_malloc(size_t n){
        malloc_handler handler;
        void* result;

        for(;;){
            handler = malloc_alloc_oom_handler;
            if(handler == 0)    THROW_BAD_ALLOC;
            (*handler)();
            result = malloc(n);
            if(result)  return result;
        }
    }

    template<int inst>
    void* malloc_alloc_template<inst>::oom_realloc(void* old,size_t n){
        malloc_handler handler;
        void* result;
        for(;;){
            handler = malloc_alloc_oom_handler;
            if(handler == 0)    THROW_BAD_ALLOC;
            (*handler)();
            result = realloc(old,n);
            if(result)  return result;
        }
    }

    using malloc_alloc = malloc_alloc_template<0>;
    
} // namespace ZMJ

#endif // sgi_allocator.h