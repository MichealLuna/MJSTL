#ifndef __SGI_CONSTRUCT_H__
#define __SGI_CONSTRUCT_H__

#ifndef USE_NEW
#define USE_NEW
#include <new>
#endif // !USE_NEW

#include "iterator.h"
#include "type_traits.h"
namespace ZMJ
{

    /*
     *  construct and destory
     */
    template <class T, class V>
    inline void construct(T *p, const V &v)
    {
        new (p) T(v);
    }

    /*对于一般泛化版本destory*/
    template <class T>
    inline void destory(T *p)
    {
        p->~T();
    }

    /*对于迭代器版本的destory*/
    template <class ForwardIterator>
    inline void destory(ForwardIterator first, ForwardIterator last)
    {
        __destory(first, last,&(*first));
    }

    template <class ForwardIterator, class T>
    inline void __destory(ForwardIterator first, ForwardIterator last, T *)
    {
        using trivial_destructor = std::has_virtual_destructor<T>;
        __destory_aux(first, last, trivial_destructor());
    }

    template <class ForwardIterator>
    inline void __destory_aux(ForwardIterator first, ForwardIterator last, std::true_type) {}

    template <class ForwardIterator>
    inline void __destory_aux(ForwardIterator first, ForwardIterator last, std::false_type)
    {
        for (; first != last; ++first)
            destory(&(*first));
    }

    /*对于字符类型destory*/
    inline void destory(char *, char *) {}
    inline void destory(wchar_t *, wchar_t *) {}
}// namespace ZMJ
#endif // !__SGI_CONSTRUCT_H__