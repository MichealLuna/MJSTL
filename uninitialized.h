#ifndef __UNINITIALIZED_H__
#define __UNINITIALIZED_H__

#include "iterator.h"
#include "type_traits.h"
#include "sgi_construct.h"
#include <wchar.h>

namespace ZMJ{

/*  API: 如果T是is_POD类型直接拷贝，否则逐一调用构造函数。
*
*   note:表示从[first,last) 复制到result开始的地方
*   template<class InputIterator, class ForwardIterator>
*   ForwardIterator 
*   uninitialized_copy(InputIterator first,InputIterator last, ForwardIterator result);
*
*   note:表示从[first,last) 都填充x。
*   template<class ForwardIterator, class T>
*   ForwardIterator
*   uninitialized_fill(ForwardIterator first , ForwardIterator last, const T& x);
*
*   note:表示从[first,first+n) 都填充x。
*   template<class ForwardIterator, class Size, class T>
*   ForwardIterator
*   unitialized_fill_n(ForwardIterator first , Size n , const T& x);
*
*/

/**********************************uninitialized_copy**********************************/
template<class InputIterator, class ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
    return __uninitialized_copy(first,last,result,value_type(result));
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last, 
        ForwardIterator result, T*){
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

/*对于is_POD型，却什么也不做，连初始化都没做，应该会在其它地方做。*/
template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
        ForwardIterator result, __true_type){
    return result;
}

/*这里做placement new其实也相当于了copy，因为会使用result开始的值来初始化[first，last)*/
template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
        ForwardIterator result, __false_type){
    ForwardIterator cur = result;
    for(; first != last; ++first, ++ cur)
        construct(&*cur,*first);
    return cur;
}

template<>
inline char* uninitialized_copy(char* first, char* last, char* result){
    memmove(result,first,last - first);
    return result + (last - first);
}

template<>
inline wchar_t* uninitialized_copy(wchar_t* first, wchar_t* last, wchar_t* result){
    memmove(result,first,(last - first)*sizeof(wchar_t));
    return result + (last - first);
}

/**********************************uninitialized_fill**********************************/
template<class ForwardIterator, class T>
inline void
uninitialized_fill(ForwardIterator first , ForwardIterator last, const T& x){
    return __uninitialized_copy(first, last, x, value_type(first));
}

template<class ForwardIterator, class T>
inline void
__uninitialized_copy(ForwardIterator first, ForwardIterator last, const T& x, T*){
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, x, is_POD());
}

template<class ForwardIterator, class T>
inline void
__uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type){
}

template<class ForwardIterator, class T>
inline void
__uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type){
    for(; first != last; ++first)
        construct(&*first, x);
}

/**********************************uninitialized_fill_n**********************************/
template<class ForwardIterator, class Size,class T>
inline ForwardIterator
uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
    return __uninitialized_fill_n(first,n,x,value_type(first));
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T*){
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first,n,x,is_POD());
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first,Size n,const T& x,__true_type){
    return first;
}

template<class ForwardIterator,class Size, class T>
inline void
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type){
    for(; n > 0; --n, ++first)
        construct(&*first, x);
    return first;
}

};//namespace ZMJ
#endif//uninitialized.h