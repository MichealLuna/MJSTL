#ifndef __ALGO_BASE_H__
#define __ALGO_BASE_H__

#ifndef USE_CSTRING
#define USE_CSTRING
#include<cstring>
#endif//! USE_CSTRING

#ifndef USE_CMATH
#define USE_CMATH
#include<cmath>
#endif//! USE_CMATH

#include "iterator.h"
#include "type_traits.h"
#include "pair.h"
#include "util.h"

namespace mjstl{

/***********************************************copy***********************************************/
/*这个copy行为只有在传入的是指针，并且是has_trivial_assignment_operator时才使用memmove。*/
/*__copy_d*/
template<class RandomAccessIterator, class OutputIterator,class Distance>
inline OutputIterator
__copy_d(RandomAccessIterator first, RandomAccessIterator last,
    OutputIterator result, Distance*){
    for(Distance n = last - first; n > 0 ; --n, ++first, ++result)
        *result = *first;
    return result;
}

/*__copy_t: 指针所指对象具备trivial_assignment_operator*/
template<class T>
inline T* __copy_t(const T* first, const T* last, T* result, __true_type){
    memmove(result,first,(size_t)(last - first)*sizeof(T));
    return result + (last - first);
}

/*__copy_t: 指针所指对象具备non-trivial_assignment_operator*/
template<class T>
inline T* __copy_t(const T* first, const T* last, T* result, __false_type){
    /*因为是指针类型，它其实random_access_iterator,所以初入一个原始指针，让编译器推导。*/
    return __copy_d(first,last,result,(ptrdiff_t*)0);
}
/*__copy的InputIterator版本*/
template<class InputIterator, class OutputIterator>
inline OutputIterator
__copy(InputIterator first, InputIterator last, 
    OutputIterator result,input_iterator_tag){
    for(; first != last; ++first,++result)
        *result = *first;
    return result;
}

/*__copy的Random_access_iterator版本*/
template<class RandomAccessIterator, class OutputIterator>
inline OutputIterator
__copy(RandomAccessIterator first, RandomAccessIterator last, 
    OutputIterator result,random_access_iterator_tag){
    return __copy_d(first,last,result,distance_type(first));
}


/*泛化版本*/
template <class InputIterator, class OutputIterator>
struct __copy_dispatch{
    OutputIterator operator()(InputIterator first, InputIterator last, 
        OutputIterator result){
        return __copy(first,last,result,iterator_category(first));
    }
};

/*两个参数都是指针类型版本*/
template<class T>
struct __copy_dispatch<T*,T*>{
    T* operator()(T* first, T* last, T* result){
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        return __copy_t(first,last,result,t());
    }
};

/*两个参数都是指针类型，但第一个指针可以是const T* 因为只读，而第二个不能是const。*/
template<class T>
struct __copy_dispatch<const T*,T*>{
    T* operator()(const T* first, const T* last, T* result){
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        return __copy_t(first,last,result,t());
    }
};




template<class InputIterator, class OutputIterator>
inline OutputIterator
copy(InputIterator first, InputIterator last, OutputIterator result){
    return __copy_dispatch<InputIterator,OutputIterator>()(first,last,result);
}

//const char* special version
inline char* copy(const char* first, const char* last, char* result){
    memmove(result,first,last-first);
    return result + (last - first);
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
    memmove(result,first,(last-first)*sizeof(wchar_t));
    return result + (last - first);
}

/***********************************************copy_backward***********************************************/
/*
*   用来将元素从背后开始复制，范围也满足左闭右开约定。例如：
*   [first,last) 区间复制到 [result - (last - first),result);其中result是要复制到的地方的尾部，符合左闭右开。
*/

/*__copy_backward: bidirectional_iterator */
template<class BidirectionalIterator1,class BidirectionalIterator2,class Distance>
inline BidirectionalIterator2
__copy_backward(BidirectionalIterator1 first,BidirectionalIterator1 last,
    BidirectionalIterator2 result,bidirectional_iterator_tag,Distance*){
    while(first != last)
        *--result = *--last;
    return result;
}

/*__copy_backward: random_access_iterator */
template<class RandomAccessIterator1,class RandomAccessIterator2,class Distance>
inline RandomAccessIterator2
__copy_backward(RandomAccessIterator1 first,RandomAccessIterator1 last,
    RandomAccessIterator2 result,random_access_iterator_tag,Distance*){
    for(Distance n = last - first; n > 0; --n)
        *--result = *--last;
    return result;
}

/*__copy_backward_dispatch泛化*/
template<class BidirectionalIterator1, class BidirectionalIterator2,class Type>
struct __copy_backward_dispatch{
    typedef typename iterator_traits<BidirectionalIterator1>::iterator_category category;
    typedef typename iterator_traits<BidirectionalIterator1>::difference_type distance;

    static BidirectionalIterator2 copy(BidirectionalIterator1 first,BidirectionalIterator1 last,
        BidirectionalIterator2 result){
        return __copy_backward(first,last,result,category(),(distance*)0);
    }
};

/*__copy_backward_dispatch： 两个指针T* */
template<class T>
struct __copy_backward_dispatch<T*,T*,__true_type>{
    static T* copy(T* first,T* last,T* result){
        const ptrdiff_t n = last - first;
        memmove(result-n,first,n*sizeof(T));
        return result-n;
    }
};

template<class T>
struct __copy_backward_dispatch<const T*,T*,__true_type>{
    static const T* copy(const T* first,const T* last,T* result){
        const ptrdiff_t n = last - first;
        return __copy_backward_dispatch<T*,T*,__true_type>::copy(first,last,result);
    }
};

template<class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2
copy_backward(BidirectionalIterator1 first,BidirectionalIterator1 last,
    BidirectionalIterator2 result){
    typedef typename iterator_traits<BidirectionalIterator1>::value_type value_type;
    typedef typename __type_traits<value_type>::has_trivial_assignment_operator trivaial_assign;
    return __copy_backward_dispatch<BidirectionalIterator1,BidirectionalIterator2,trivaial_assign()>::
        copy(first,last,result);
}

/************************************************equal*************************************************/


/*比较第一序列[first,last) 是否和第二序列相等*/
template<class InputIterator1,class InputIterator2>
inline bool 
equal(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2){
    for(;(first1 != last1) && (first2 != last2) ; ++first1,++first2)
        if(!first2 || *first1 != *first2) return false;
    return true;
}

/*通过cmp仿函数来进行比较*/
template<class InputIterator1,class InputIterator2,class Compared>
inline bool 
equal(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,Compared cmp){
    for(;(first1 != last1) && (first2 != last2); ++first1,++first2)
        if(!first2 || !cmp(*first1,*first2)) return false;
    return true;
}

/***********************************************mismatch***********************************************/
/*返回两个序列中不匹配的第一个位置*/
template<class InputIterator1,class InputIterator2>
pair<InputIterator1,InputIterator2>
mismatch(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2){
    while(first1 != last1 && *first1 == *first2){
        ++first1;
        ++first2;
    }
    return pair<InputIterator1,InputIterator2>(first1,first2);
}

/*使用cmp作为比较函数*/
template<class InputIterator1,class InputIterator2,class Compared>
pair<InputIterator1,InputIterator2>
mismatch(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2,Compared cmp){
    while(first1 != last1 && cmp(*first1,*first2)){
        ++first1;
        ++first2;
    }
    return pair<InputIterator1,InputIterator2>(first1,first2);
}


/***********************************************fill***********************************************/
template<class ForwardIterator,class T>
void fill(ForwardIterator first,ForwardIterator last,const T& value){
    for(;first != last; ++first)
        *first = value;
}

template<class ForwardIterator,class Size,class T>
ForwardIterator fill_n(ForwardIterator first,Size n, const T& value){
    for(;n > 0; --n,++first)
        *first = value;
    return first;
}

void fill(unsigned char* first,unsigned char* last,unsigned char& value){
    unsigned char tmp = value;
    memset(first,tmp,(last-first));
}

void fill(signed char* first,signed char* last,signed char& value){
    signed char tmp = value;
    memset(first,static_cast<unsigned char>(tmp),(last-first));
}

void fill(char* first,char* last,const char& value){
    char tmp = value;
    memset(first,static_cast<unsigned char>(tmp),(last-first));
}

template<class Size>
unsigned char* fill_n(unsigned char* first,Size n,unsigned char& value){
    fill(first,first+n,value);
    return first + n;
}

template<class Size>
signed char* fill_n(signed char* first,Size n,signed char& value){
    fill(first,first+n,value);
    return first + n;
}

template<class Size>
char* fill_n(char* first,Size n,char& value){
    fill(first,first+n,value);
    return first + n;
}

/********************************************unchecked_move********************************************/
template<class InputIterator,class OutputIterator>
OutputIterator 
unchecked_move_cat(InputIterator first,InputIterator last,
    OutputIterator result,mjstl::input_iterator_tag){
    for(;first != last; ++first,++result)
        *result = mjstl::move(*first);
    return result;
}

template<class InputIterator,class OutputIterator>
OutputIterator 
unchecked_move_cat(InputIterator first,InputIterator last,
    OutputIterator result,mjstl::random_access_iterator_tag){
    for(auto n = last - first; n > 0; --n,++first,++result)
        *result = mjstl::move(*first);
    return result;
}

template<class InputIterator,class OutputIterator>
OutputIterator 
unchecked_move(InputIterator first,InputIterator last,OutputIterator result){
    return unchecked_move_cat(first,last,result,iterator_category(first));
}

template<class Tp,class Up>
typename std::enable_if<
  std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
  std::is_trivially_move_assignable<Up>::value,
  Up*>::type 
unchecked_move(Tp* first,Tp* last,Up* result){
    const size_t n = static_cast<size_t>(last - first);
    if(n != 0)
        std::memmove(result,first,n);
    return result + n;
}


/***********************************************iter_swap***********************************************/
/*交换两个ForwardIterator 所指对象*/
template<class ForwardIterator1,class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a,ForwardIterator2 b){
    typename iterator_traits<ForwardIterator1>::value_type tmp = *a;
    *a = *b;
    *b = tmp;
}

/***********************************************swap***********************************************/
template<class T>
inline void swap(T& a,T& b){
    T tmp = a;
    a = b;
    b = tmp;
}

/*********************************lexicographical_compare****************************************/
template<class InputIterator1,class InputIterator2>
bool lexicographical_compare(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2){
    for(;first1 != last1 && first2 != last2; ++first1, ++first2){
        if(*first1 < *first2)
            return true;
        if(*first2 < *first1)
            return false;
    }
    return first1 == last1 && first2 != last2;
}

template<class InputIterator1,class InputIterator2,class Compare>
bool lexicographical_compare(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,Compare comparator){
    for(;first1 != last1 && first2 != last2; ++first1, ++first2){
        if(comparator(*first1,*first2))
            return true;
        if(comparator(*first2,*first1))
            return false;
    }
    return first1 == last1 && first2 != last2;
}

template<class T>
inline const T& max(const T& a,const T& b){
    return a > b ? a : b;
}

template<class T,class Compare>
inline const T& max(const T& a,const T& b,Compare comp){
    return comp(a,b) ? a : b;
}

template<class T>
inline const T& min(const T& a,const T& b){
    return a < b ? a : b;
}

template<class T,class Compare>
inline const T& min(const T& a,const T& b,Compare comp){
    return comp(a,b) ? b : a;
}

inline bool lexicographical_compare(const unsigned char* first1,const unsigned char* last1,
    const unsigned char* first2,const unsigned char* last2){
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = ::memcmp(first1,first2,min(len1,len2));
    return result != 0 ? result < 0 : len1 < len2;
}

template<class T>
inline T abs(T value){
    return value > 0 ? value : -value;
}

template<> 
inline float abs<float>(float value){
    return std::fabs(value);
}

template<> 
inline double abs<double>(double value){
    return std::fabs(value);
}


}//namespace mjstl

#endif//algobase.h