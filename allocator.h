#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "sgi_construct.h"
#include "sgi_allocator.h"
namespace mjstl{
template <class T>
class allocator{
public:
    typedef  T          value_type;
    typedef  T*         pointer;
    typedef  const T*   const_pointer;
    typedef  T&         reference;
    typedef  const T&   const_reference;
    typedef  size_t     size_type;
    typedef  ptrdiff_t  difference_type;

public:
    template <class U>
    struct rebind{
        using other = allocator<U>;
    };

public:
    static T* allocate();
    static T* allocate(size_t n);
    static void deallocate(T* ptr);
    static void deallocate(T* ptr,size_t n);

    static void construct(T* ptr);
    static void construct(T* ptr,const T& x);
    static void destory(T* ptr);
    static void destory(T* ptr,T* last);
};


template<class T>
T* allocator<T>::allocate(){
    return static_cast<T*>(alloc::allocate(sizeof(T)));
}

template<class T>
T* allocator<T>::allocate(size_t n){
    if(n == 0) return 0;
    return static_cast<T*>(alloc::allocate(n * sizeof(T)));
}

template<class T>
void allocator<T>::deallocate(T* ptr){
    if(ptr == 0) return;
    alloc::deallocate(ptr,sizeof(T));
}

template<class T>
void allocator<T>::deallocate(T* ptr,size_t n){
    if(ptr == 0) return;
    alloc::deallocate(ptr,n * sizeof(T));
}

template<class T>
void allocator<T>::construct(T* ptr){
    mjstl::construct(ptr);
}

template<class T>
void allocator<T>::construct(T* ptr,const T& x){
    mjstl::construct(ptr,x);
}

template<class T>
void allocator<T>::destory(T* ptr){
    mjstl::destory(ptr);
}

template<class T>
void allocator<T>::destory(T* first,T* last){
    mjstl::destory(first,last);
}


}// namespace mjstl
#endif// !__ALLOCATOR_H__