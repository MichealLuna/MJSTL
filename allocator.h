#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <new>          // for placement new
#include <cstddef>      // for ptrdiff_t size_t
#include <cstdlib>      // for exit
#include <climits>      // for UINT_MAX
#include <iostream>     // cerr

namespace ZMJ{


template <class T>
inline T* __allocate(ptrdiff_t size, T*){
    std::set_new_handler(0);
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if (tmp == 0) {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template <class T>
inline void __deallocate(T* buffer){
    ::operator delete(buffer);
}

template <class T,class V>
inline void __construct(T* p, const V& value){
    new(p) T(value);
}

template <class T>
inline void __destroy(T* p){
    p->~T();
}

template <class T>
class allocator{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    template <class U>
    struct rebind{
        using other = allocator<U>;
    };

    //for memory
    pointer allocate(size_type n, const void* hint = 0){
        return __allocate((difference_type)n,(pointer)hint);
    }
    //for release memory
    void deallocate(pointer p, size_type n){
        __deallocate(p);
    }

    //for initalize memory
    void construct(pointer p,const T& value){
        __construct(p,value);
    }
    //for release resource in this type T.
    void destory(pointer p){
        __destroy(p);
    }

    pointer address(reference x){
        return (pointer)&x;
    }

    const_pointer address(const_reference x){
        return (const_pointer)(&x);
    }

    size_type max_size(){
        return (size_type)(UINT_MAX / sizeof(T));
    }
};

}

#endif//allocator.h