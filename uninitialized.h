#ifndef __UNINITIALIZED_H__
#define __UNINITIALIZED_H__

#include "iterator.h"
#include "type_traits.h"
#include "sgi_construct.h"
#include "algobase.h"
#include "util.h"

namespace mjstl{

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
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
        ForwardIterator result, __true_type){
    return copy(first,last,result);
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

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last, 
        ForwardIterator result, T*){
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
    return __uninitialized_copy(first,last,result,value_type(result));
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
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type){
    fill(first,last,x);
}

template<class ForwardIterator, class T>
inline void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type){
    for(; first != last; ++first)
        construct(&*first, x);
}

template<class ForwardIterator, class T>
inline void
__uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T*){
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_fill_aux(first, last, x, is_POD());
}

template<class ForwardIterator, class T>
inline void
uninitialized_fill(ForwardIterator first , ForwardIterator last, const T& x){
    return __uninitialized_fill(first, last, x, value_type(first));
}

/**********************************uninitialized_fill_n**********************************/
template<class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first,Size n,const T& x,__true_type){
    return fill_n(first,n,x);
}

template<class ForwardIterator,class Size, class T>
inline void
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type){
    for(; n > 0; --n, ++first)
        construct(&*first, x);
    return first;
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T*){
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first,n,x,is_POD());
}

template<class ForwardIterator, class Size,class T>
inline ForwardIterator
uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
    return __uninitialized_fill_n(first,n,x,value_type(first));
}

template<class InputIterator,class ForwardIterator>
ForwardIterator
unchecked_uninit_move(InputIterator first,InputIterator last,
    ForwardIterator result,std::false_type){
    ForwardIterator cur = result;
    try{
        for(;first != last; ++first,++cur)
            mjstl::construct(&*cur,mjstl::move(*first));
    }catch(...){
        mjstl::destory(result,cur);
    }
    return cur;
}

template<class InputIterator,class ForwardIterator>
ForwardIterator
unchecked_uninit_move(InputIterator first,InputIterator last,
    ForwardIterator result,std::true_type){
    return mjstl::move(first,last,result);
}

template<class InputIterator,class ForwardIterator>
ForwardIterator 
uninitialized_move(InputIterator first,InputIterator last,ForwardIterator result){
    return unchecked_uninit_move(first,last,result,
        std::is_trivially_move_assignable<typename iterator_traits<
            InputIterator>::value_type>{});
}


/*
*  debug:
*  1、这里uninitialized_copy跟uninitialized_fill的辅助函数名冲突了，
*  导致编译器报is ambiguous。（仔细看报错note：提示两个地方的候选
*  模板，才发现重命名了。）
*/

};//namespace ZMJ
#endif//uninitialized.h