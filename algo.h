#ifndef __ALGO_H__
#define __ALGO_H__

#ifndef USE_CSTDLIB_H
#define USE_CSTDLIB_H
#include <cstdlib>
#endif//!USE_CSTDLIB_H

#ifndef USE_CTIME_H
#define USE_CTIME_H
#include <ctime>
#endif//!USE_CTIME_H

#include "iterator.h"
#include "algobase.h"
#include "heap_algo.h"
#include "reverse_iterator.h"

namespace ZMJ{

/***********************************************adjacent_find*************************************************
*   找到第一个相邻重复元素，返回这对元素的第一个元素的迭代器。
*************************************************************************************************************/
template<class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last){
    if(first == last) return last; /*为空，返回last表示未找到*/
    ForwardIterator next = first;
    while(++next != last){
        if(*next == *first)
            return first;
        first = next;
    }
    return last;
}

template<class ForwardIterator,class Compare>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, Compare comp){
    if(first == last) return last;
    auto next = first;
    while(++next != last){
        if(comp(*next,*first))
            return first;
        first = next;
    }
    return last;
}

/*********************************************upper_bound**********************************************
*   二分查找，如果在[first,last)最后一个同于value的元素，并返回该位置。
*******************************************************************************************************/
/*__upper_bound forward_iterator版本*/
template<class ForwardIterator,class T,class Distance>
ForwardIterator 
__upper_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Distance*,forward_iterator_tag){
    Distance len = distance(first,last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle,half);
        if(value < *middle){
            len = half;
        }else{
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template<class RandomAccessIterator,class T,class Distance>
RandomAccessIterator 
__upper_bound(RandomAccessIterator first,RandomAccessIterator last,
    const T& value,Distance*,random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(value < *middle){
            len = half;
        }else{
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template<class ForwardIterator,class T>
ForwardIterator
upper_bound(ForwardIterator first,ForwardIterator last,const T& value){
    return __upper_bound(first,last,value,
        distance_type(first),iterator_category(first));
}

/*upper_bound仿函数版本。*/
template<class ForwardIterator,class T,class Distance,class Compare>
ForwardIterator 
__upper_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Compare comp,Distance*,forward_iterator_tag){
    Distance len = distance(first,last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle,half);
        if(comp(value,*middle)){
            len = half;
        }else{
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template<class RandomAccessIterator,class T,class Distance,class Compare>
RandomAccessIterator 
__upper_bound(RandomAccessIterator first,RandomAccessIterator last,
    const T& value,Compare comp,Distance*,random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(comp(value,*middle)){
            len = half;
        }else{
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template<class ForwardIterator,class T,class Compare>
ForwardIterator
upper_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Compare comp){
    return __upper_bound(first,last,value,comp,
        distance_type(first),iterator_category(first));
}

/*********************************************lower_bound**********************************************
*   二分查找，如果在[first,last)第一个同于value的元素，并返回该位置。
*******************************************************************************************************/

/*__lower_bound，forward_iterator版本*/
template<class ForwardIterator,class T,class Distance>
ForwardIterator
__lower_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Distance*,forward_iterator_tag){
    Distance len = distance(first,last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle,half);
        if(*middle < value){
            first = middle;
            ++first;
            len = len - half - 1;/*middle也可以排除，多减1。*/
        }else
            len = half;
    }
    return first;
}

/*__lower_bound, random_access_iterator版本*/
template<class ForwardIterator,class T,class Distance>
ForwardIterator
__lower_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Distance*,random_access_iterator_tag){
    Distance len = last - len;
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(*middle < value){
            first = middle + 1;
            len = len - half - 1;
        }else
            len = half;
    }
    return first;/*len 长度为0，说明二分的上下边界被缩小到0，first就被确定了*/
}

template<class ForwardIterator,class T>
inline ForwardIterator 
lower_bound(ForwardIterator first,ForwardIterator last,const T& value){
    return __lower_bound(first,last,value,
    distance_type(first),iterator_category(first));
}

/*重载lower_bound,使用comp仿函数进行比较*/
/*__lower_bound的仿函数，forward_iterator版本*/
template<class ForwardIterator,class T,class Distance,class Compare>
ForwardIterator
__lower_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Compare comp,Distance*,forward_iterator_tag){
    Distance len = distance(first,last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle,half);
        if(comp(*middle,value)){
            first = middle;
            ++first;
            len = len - half - 1;
        }else
            len = half;
    }
    return first;
}

/*__lower_bound, random_access_iterator版本*/
template<class ForwardIterator,class T,class Distance,class Compare>
ForwardIterator
__lower_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Compare comp,Distance*,random_access_iterator_tag){
    Distance len = last - len;
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(comp(*middle,value)){
            first = middle + 1;
            len = len - half - 1;
        }else
            len = half;
    }
    return first;
}

template<class ForwardIterator,class T,class Compare>
inline ForwardIterator 
lower_bound(ForwardIterator first,ForwardIterator last,
    const T& value,Compare comp){
    return __lower_bound(first,last,value,comp,
    distance_type(first),iterator_category(first));
}


/*********************************************binary_search********************************************
*   二分查找，如果在[first,last)有等同于value的元素，返回true,否则返回false。
*******************************************************************************************************/
template<class ForwardIterator,class T>
bool binary_search(ForwardIterator first,ForwardIterator last,const T& value){
    ForwardIterator it = lower_bound(first,last,value);
    return it != last && !(value < *it);
}

template<class ForwardIterator,class T,class Compare>
bool binary_search(ForwardIterator first,ForwardIterator last,
    const T& value,Compare comp){
    ForwardIterator it = lower_bound(first,last,value);
    return it != last && !comp(value,*it);
}

/***********************************************count************************************************
*   计算容器[first,last)范围内跟输入参数value相等元素个数。
*****************************************************************************************************/
template<class InputIterator,class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first,InputIterator last,const T& value){
    typename itearator_traits<InputIterator>::difference_type ret = 0;
    while(first != last){
        if(*first == value)
            ++ret;
        ++first;
    }
    return ret;
}

/***********************************************count_if************************************************
*   对容器[first,last)范围进行comp操作，返回操作成功的个数。
********************************************************************************************************/
template<class InputIterator,class Compare>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first,InputIterator last,Compare comp){
    typename iterator_traits<InputIterator>::difference_type ret = 0;
    while(first != last){
        if(comp(*first))
            ++ret;
        ++first;
    }
    return ret;
}

/*********************************************equal_range***********************************************
*   返回[first,last)区间与value相等的区间，并返回一对迭代器指向区间首尾。
********************************************************************************************************/
/*__equal_range的 forward_iterator版本*/
template<class ForwardIterator,class T,class Distance>
pair<ForwardIterator,ForwardIterator>
__equal_range(ForwardIterator first,ForwardIterator last,const T& value,
    Distance*,forward_iterator_tag){
    Distance len = distance(first,last);
    Distance half;
    ForwardIterator middle;
    
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle,half);

        /*找下边界*/
        if(value < *middle){/*排除后半段*/
            len = half;
        }else if(*middle < value){/*排除前半段*/
            first = middle;
            ++first;
            len = len - half - 1;
        }else{/*找到了一个等于value的地方*/
            //auto left = lower_bound(first,middle,value);
            auto left = lower_bound(first,last,value);/*这里似乎可以middle代替last？*/
            advance(first,len);
            auto right = upper_bound(++middle,first,value);/*first+len比last要节省时间。*/
            //auto right = upper_bound(++middle,last,value);
            return pair<ForwardIterator,ForwardIterator>(left,right);
        }
    }
    /*没有value*/
    return pair<ForwardIterator,ForwardIterator>(last,last);
}

/*__equal_range的random_access_iterator版本。*/
template<class RandomAccessIterator,class T,class Distance>
pair<RandomAccessIterator,RandomAccessIterator>
__equal_range(RandomAccessIterator first,RandomAccessIterator last,const T& value,
    Distance*,random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    
    while(len > 0){
        half = len >> 1;
        middle = first + half;

        /*找下边界*/
        if(value < *middle){/*排除后半段*/
            len = half;
        }else if(*middle < value){/*排除前半段*/
            first = middle + 1;
            len = len - half - 1;
        }else{/*找到了一个等于value的地方*/
            auto left = lower_bound(first,middle,value);/*这里又可以使用middle??*/
            auto right = upper_bound(++middle,first + len,value);
            return pair<ForwardIterator,ForwardIterator>(left,right);
        }
    }
    /*没有value*/
    return pair<ForwardIterator,ForwardIterator>(last,last);
}

template<class ForwardIterator,class T>
pair<ForwardIterator,ForwardIterator>
equal_range(ForwardIterator first,ForwardIterator last,const T& value){
    return __equal_range(first,last,value,distance(first),iterator_category(first));
}

/*equal_range使用仿函数comp版本。*/
/*__equal_range使用仿函数comp的 forward_iterator版本*/
template<class ForwardIterator,class T,class Distance,class Compare>
pair<ForwardIterator,ForwardIterator>
__equal_range(ForwardIterator first,ForwardIterator last,const T& value,
    Compare comp,Distance*,forward_iterator_tag){
            Distance len = distance(first,last);
    Distance half;
    ForwardIterator middle;
    
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle,half);

        if(comp(value,*middle)){
            len = half;
        }else if(comp(*middle,value)){
            first = middle;
            ++first;
            len = len - half - 1;
        }else{
            auto left = lower_bound(first,last,value,comp);
            advance(first,len);
            auto right = upper_bound(++middle,first,value,comp);
            return pair<ForwardIterator,ForwardIterator>(left,right);
        }
    }
    return pair<ForwardIterator,ForwardIterator>(last,last);
}

/*__equal_range使用仿函数comp的random_access_iterator版本。*/
template<class RandomAccessIterator,class T,class Distance,class Compare>
pair<RandomAccessIterator,RandomAccessIterator>
__equal_range(RandomAccessIterator first,RandomAccessIterator last,const T& value,
    Compare comp,Distance*,random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    
    while(len > 0){
        half = len >> 1;
        middle = first + half;

        if(comp(value,*middle)){
            len = half;
        }else if(comp(*middle,value)){
            first = middle + 1;
            len = len - half - 1;
        }else{
            auto left = lower_bound(first,middle,value,comp);
            auto right = upper_bound(++middle,first + len,value,comp);
            return pair<ForwardIterator,ForwardIterator>(left,right);
        }
    }
    return pair<ForwardIterator,ForwardIterator>(last,last);
}

template<class ForwardIterator,class T,class Compare>
pair<ForwardIterator,ForwardIterator>
equal_range(ForwardIterator first,ForwardIterator last,const T& value,Compare comp){
    return __equal_range(first,last,value,comp,distance(first),iterator_category(first));
}

/***********************************************find************************************************
*   查找容器[first,last)范围第一个找到的元素value，并返回找到元素的迭代器。
********************************************************************************************************/
template<class InputIterator,class T>
InputIterator find(InputIterator first,InputIterator last,const T& value){
    while(first != last && *first != value) ++first;
    return first;
}

/***********************************************find_if************************************************
*   查找容器[first,last)范围第一个使得输入自定义函数comp为true的元素，并返回找到元素的迭代器。
********************************************************************************************************/
template<class InputIterator,class Compare>
InputIterator find(InputIterator first,InputIterator last,Compare comp){
    while(first != last && !comp(*first))  ++first;
    return first;
}

/***********************************************find_end************************************************
*  在区间[first1,last1)区间中查找[first2,last2)最后一次出现的地方，若不存在返回last1。
********************************************************************************************************/
template<class ForwardIterator1,class ForwardIterator2>
inline ForwardIterator1 find_end(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2){
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
    typedef typename iterator_traits<ForwardIterator2>::iterator_category category2;
    return __find_end(first1,last1,first2,last2,category1(),category2());
}

template<class ForwardIterator1,class ForwardIterator2>
ForwardIterator1 __find_end(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2,
    forward_iterator_tag,forward_iterator_tag){
    if(first2 == last2) return last1;
    else{
        ForwardIterator1 result = last1;
        while(true){
            ForwardIterator1 new_result = search(first1,last1,first2,last2);
            if(new_result == last1) return result;
            else{
                /*要记录最新结果位置，最终返回最后一个位置的结果*/
                result = new_result;
                first1 = new_result;
                ++first1;
            }
        }
    }
}

template<class BidirectionalIterator1,class BidirectionalIterator2>
BidirectionalIterator1 __find_end(BidirectionalIterator1 first1,BidirectionalIterator1 last1,
    BidirectionalIterator2 first2,BidirectionalIterator2 last2,
    bidirectional_iterator_tag,bidirectional_iterator_tag){
    typedef reverse_iterator<BidirectionalIterator1> reverseIterator1;
    typedef reverse_iterator<BidirectionalIterator2> reverseIterator2;

    /*具有双向性的容器，可以从后往前匹配，那么只要找到第一个匹配，相当从前面开始匹配的最后一个。*/
    reverseIterator1 rlast1(first1);
    reverseIterator2 rlast2(first2);
    reverseIterator1 rresult = search(rlast1,last1,rlast2,last2);

    if(rresult == last1) return last1;
    else{
        BidirectionalIterator1 result = rresult.base();/*变回正向迭代器？*/
        advance(result,-(rlast2 - first2));/*逆匹配找到的是结尾，所以要减去长度，变回开头*/
        return result;
    }
}

template<class ForwardIterator1,class ForwardIterator2,class Compare>
inline ForwardIterator1 find_end(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2,Compare comp){
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
    typedef typename iterator_traits<ForwardIterator2>::iterator_category category2;
    return __find_end(first1,last1,first2,last2,category1(),category2(),comp);
}

template<class ForwardIterator1,class ForwardIterator2,class Compare>
ForwardIterator1 __find_end(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2,
    forward_iterator_tag,forward_iterator_tag,Compare comp){
    if(first2 == last2) return last1;
    else{
        ForwardIterator1 result = last1;
        while(true){
            ForwardIterator1 new_result = search(first1,last1,first2,last2,comp);
            if(new_result == last1) return result;
            else{
                /*要记录最新结果位置，最终返回最后一个位置的结果*/
                result = new_result;
                first1 = new_result;
                ++first1;
            }
        }
    }
}

template<class BidirectionalIterator1,class BidirectionalIterator2,class Compare>
BidirectionalIterator1 __find_end(BidirectionalIterator1 first1,BidirectionalIterator1 last1,
    BidirectionalIterator2 first2,BidirectionalIterator2 last2,
    bidirectional_iterator_tag,bidirectional_iterator_tag,Compare comp){
    typedef reverse_iterator<BidirectionalIterator1> reverseIterator1;
    typedef reverse_iterator<BidirectionalIterator2> reverseIterator2;

    /*具有双向性的容器，可以从后往前匹配，那么只要找到第一个匹配，相当从前面开始匹配的最后一个。*/
    reverseIterator1 rlast1(first1);
    reverseIterator2 rlast2(first2);
    reverseIterator1 rresult = search(rlast1,last1,rlast2,last2,comp);

    if(rresult == last1) return last1;
    else{
        BidirectionalIterator1 result = rresult.base();/*变回正向迭代器？*/
        advance(result,-(rlast2 - first2));/*逆匹配找到的是结尾，所以要减去长度，变回开头*/
        return result;
    }
}

/***********************************************find_first_of************************************************
*  返回[first1,last1)中第一次出现[first2,last2)中任意元素的位置，若不存在返回last1。
************************************************************************************************************/
template<class InputIterator,class ForwardIterator>
InputIterator find_first_of(InputIterator first1,InputIterator last1,
    ForwardIterator first2,ForwardIterator last2){
    for(;first1 != last1; ++first1)
        for(ForwardIterator it = first2; it != last2; ++it)
            if(*first1 == *it) return first1;
    return last1;
}

template<class InputIterator,class ForwardIterator,class Compare>
InputIterator find_first_of(InputIterator first1,InputIterator last1,
    ForwardIterator first2,ForwardIterator last2,Compare comp){
    for(;first1 != last1; ++first1)
        for(ForwardIterator it = first2; it != last2; ++it)
            if(comp(*first1,*it)) return first1;
    return last1;
}

/***********************************************for_each****************************************************
*  对[first1,last1)所有元素执行func()操作，但不能改变元素内容，func()可以返回一个值，但会被忽略。
************************************************************************************************************/
template<class InputIterator,class Function>
Function for_each(InputIterator first,InputIterator last,Function func){
    for(;first != last; ++first)
        func(*first);
    return func;
}

/***********************************************generate****************************************************
*  使用仿函数gen，对[first1,last1)元素进行赋值。
************************************************************************************************************/
template<class InputIterator,class Generate>
void generate(InputIterator first,InputIterator last,Generate gen){
    for(;first != last; ++first)
        *first = gen();
}

/***********************************************generate_n****************************************************
*  使用仿函数gen，连续从first开始连续对n个元素赋值。
************************************************************************************************************/
template<class InputIterator,class Size,class Generate>
void generate(InputIterator first,Size n,Generate gen){
    for(;n > 0; --n)
        *first = gen();
}

/***********************************************includes****************************************************
*  判断序列S2是否被序列S1包含(<=)，S1,S2都必须是有序集合。
************************************************************************************************************/
template<class InputIterator1,class InputIterator2>
bool includes(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2,InputIterator2 last2){
    while(first1 != last1 && first2 != last2){
        if(*first2 < *first1)
            return false;
        else if(*first1 < *first2)
            ++first1;
        else
            ++first1,++first2;
    }
    return first2 == last2;
}

template<class InputIterator1,class InputIterator2,class Compare>
bool includes(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first2,*first1))
            return false;
        else if(comp(*first1,*first2))
            ++first1;
        else
            ++first1,++first2;
    }
    return first2 == last2;
}

/***********************************************max_element****************************************************
*  返回指向[first,last)中最大元素的迭代器。
**************************************************************************************************************/
template<class ForwardIterator>
ForwardIterator max_element(ForwardIterator first,ForwardIterator last){
    ForwardIterator result = first;
    for(;first != last; ++first)
        if(*result < *first) result = first;
    return result;
}

template<class ForwardIterator,class Compare>
ForwardIterator max_element(ForwardIterator first,ForwardIterator last,Compare comp){
    ForwardIterator result = first;
    for(;first != last; ++first)
        if(comp(*result,*first)) result = first;
    return result;
}

/***********************************************min_element****************************************************
*  返回指向[first,last)中最小元素的迭代器。
**************************************************************************************************************/
template<class ForwardIterator>
ForwardIterator min_element(ForwardIterator first,ForwardIterator last){
    ForwardIterator result = first;
    for(;first != last; ++first)
        if(*first < *result) result = first;
    return result;
}

template<class ForwardIterator,class Compare>
ForwardIterator min_element(ForwardIterator first,ForwardIterator last,Compare comp){
    ForwardIterator result = first;
    for(;first != last; ++first)
        if(comp(*first,*result)) result = first;
    return result;
}

/*************************************************median******************************************************
*  找出三个值的中间值。
**************************************************************************************************************/
template<class T>
inline T median(const T& l,const T& m,const T& r){
    if(l < m){
        if(m < r)       /* l < m < r*/
            return l;
        else if(l < r)  /* l < r <= m 根据上一个if条件m<r不满足所以 r <= m */ 
            return r;
        else            /* r <= l < m 根据上一个if条件不满足，所以m不可能等于r*/
            return m;
    }else if(l < r){ /*前一个if条件不满足得： m <= l, 又 l < r,所以 m <= l < r*/
        return l;
    }else if(m < r){/*由上个if条件不满足： l >= m, l >= r, 又 m < r 得 m < r <= l*/
        return r;
    }else{ /*前面条件都不满足，则： l >= m , l >= r, m >= r,得  r <= m <= l*/
        return m;
    }
}

/*median 仿函数版本comp。*/
template<class T,class Compare>
inline T median(const T& l,const T& m,const T& r,Compare comp){
    if(comp(l, m)){
        if(comp(m,r))       /* l < m < r*/
            return l;
        else if(comp(l,r))  /* l < r <= m 根据上一个if条件m<r不满足所以 r <= m */ 
            return r;
        else            /* r <= l < m 根据上一个if条件不满足，所以m不可能等于r*/
            return m;
    }else if(comp(l,r)){ /*前一个if条件不满足得： m <= l, 又 l < r,所以 m <= l < r*/
        return l;
    }else if(comp(m,r)){/*由上个if条件不满足： l >= m, l >= r, 又 m < r 得 m < r <= l*/
        return r;
    }else{ /*前面条件都不满足，则： l >= m , l >= r, m >= r,得  r <= m <= l*/
        return m;
    }
}

/**************************************************merge******************************************************
*  将两个有序集合S1，S2合并，并输出到另外一个容器。返回迭代器指向最后一个元素下一个位置。
**************************************************************************************************************/
template<class InputIterator1,class InputIterator2,class OutputIterator>
OutputIterator merge(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result){
    while(first1 != last1 && first2 != last2){
        if(*first1 < *first2){
            *result = *first1;
            ++first1;
        }else{
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    return copy(first1,last1,copy(first2,last2,result));
}

template<class InputIterator1,class InputIterator2,class OutputIterator,class Compare>
OutputIterator merge(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result,Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first1,*first2)){
            *result = *first1;
            ++first1;
        }else{
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    return copy(first1,last1,copy(first2,last2,result));
}


/*********************************************next_permutation***************************************************
*  使得[first,last)序列的变成下一个排列组合，如果没有下一个排序组合，返回false，有则返回true。
*  列如：序列[first,last)为数组： 5 1 2 9 , 它的下一序列为： 5 1 9 2，它的最大序列：9 5 2 1。
*****************************************************************************************************************/
template<class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first,BidirectionalIterator last){
    BidirectionalIterator it = last;
    if(first == last || first == --it) return false;
    for(;;){ 
        /* 
        *    if条件：旨在找到一个元素位置，这个位置元素小于后面位置的元素。
        *  这个位置特殊，因为这个位置元素小于，只要把它后面比它大的元素交换，
        *  整个序列一定是变大的。
        *    iit从last-1往前，如果多次成立，那么[iit,last)将是递减序列。 
        */
        auto iit = it;
        if(*--it < *iit){
            auto jit = last;
            /*
            *    需要在[iit,last)找到第一个比it大的元素，因为要使得序列变大，it位置新的元素
            * 当然要大于原来it位置的元素。it位置元素变大后，整个序列自然变大了，然而可能大
            * 过头，因为[iit,last)现在是最大序列，it位置变大之后，应该把[iit,last)
            * 变成最小序列，这只需要把[iit,last)进行翻转即可！
            */
            while(!(*it < *--jit)){}
            iter_swap(it,jit);
            reverse(iit,last);
            return true;
        }

        /*
        *      迭代器it一直从last-1往前直到first，说明[first,last)是一个递减序列，
        *  也就是说序列最大元素在前面，依次往后都是小于等于前一个元素，这是一个最大
        *  序列，返回false。(这里实现时，最大序列会变成最小序列)
        */
        if(it == first){
            reverse(first,last);
            return false;
        }
    }
}

template<class BidirectionalIterator,class Compare>
bool next_permutation(BidirectionalIterator first,BidirectionalIterator last,Compare comp){
    BidirectionalIterator it = last;
    if(first == last || first == --it) return false;
    for(;;){ 
        auto iit = it;
        --it;
        if(comp(*it,*iit)){
            auto jit = last;
            while(!comp(*it,*jit)){}
            iter_swap(it,jit);
            reverse(iit,last);
            return true;
        }

        if(it == first){
            reverse(first,last);
            return false;
        }
    }
}

/************************************************partial_sort****************************************************
*  对整个序列做部分排序，保证较小的N个元素以递增顺序位于[first,first + N);
*****************************************************************************************************************/
template<class RandomAccessIterator,class T>
void __partial_sort(RandomAccessIterator first,RandomAccessIterator middle,RandomAccessIterator last){
    make_heap(first,middle);
    /*
    *   建好前半段的堆，然后依次判断后半段是否小于堆顶元素，
    * 如果小于堆顶元素，说明这个first位置元素应该放到后半段，
    * 而小于堆顶元素被放入前半段的堆中。
    */
    for(RandomAccessIterator it = middle; it < last; ++it){
        if(*it < *first)
            pop_heap_aux(first,middle,it,T(*it),distance_type(first));
    }
    /*最后，[first,middle)只满足堆序性，需要重新排序。*/
    sort_heap(first,middle);
}

template<class RandomAccessIterator>
void partial_sort(RandomAccessIterator first,RandomAccessIterator middle,RandomAccessIterator last){
    __partial_sort(first,middle,last);
}

/*使用仿函数comp版本*/
template<class RandomAccessIterator,class T,class Compare>
void __partial_sort(RandomAccessIterator first,RandomAccessIterator middle,
    RandomAccessIterator last,Compare comp,T*){
    make_heap(first,middle,comp);
    for(RandomAccessIterator it = middle; it < last; ++it){
        if(*it < *first)
            pop_heap_aux(first,middle,it,T(*it),distance_type(first),comp);
    }
    sort_heap(first,middle,comp);
}

template<class RandomAccessIterator,class Compare>
void partial_sort(RandomAccessIterator first,RandomAccessIterator middle,
    RandomAccessIterator last,Compare comp){
    __partial_sort(first,middle,last,comp,value_type(first));
}

/*******************************************partial_sort_copy************************************************
*  与partial_sort相同，但将排序结果放入result。
*************************************************************************************************************/
template<class InputIterator,class RandomAccessIterator,class Distance,class T>
RandomAccessIterator
__partial_sort_copy(InputIterator first,InputIterator last,
    RandomAccessIterator result_first,RandomAccessIterator result_last,Distance*,T*){
    /*result容器可能小于输入容器。*/
    if(result_first == result_last) return result_last;
    RandomAccessIterator result_iter = result_first;
    while(first != last && result_iter != result_last){
        *result_iter = *first;
        ++first;
        ++result_iter;
    }

    make_heap(result_first,result_iter);
    for(;first != last; ++first){
        if(*first < *result_first)
            adjust_heap(result_first,static_cast<Distance(0),
                static_cast<Distance>(result_last - result_first),(T)(*first));
    }
    sort_heap(result_first,result_iter);
    return result_iter;
}

template<class InputIterator,class RandomAccessIterator>
RandomAccessIterator 
partial_sort_copy(InputIterator first,InputIterator last,
    RandomAccessIterator result_first,RandomAccessIterator result_last){
    return __partial_sort_copy(first,last,result_first,result_last,
        distance(result_first),value_type(first));
}

/*partial_sort_copy使用仿函数comp重载版本*/
template<class InputIterator,class RandomAccessIterator,
    class Compare,class Distance,class T>
RandomAccessIterator
__partial_sort_copy(InputIterator first,InputIterator last,
    RandomAccessIterator result_first,RandomAccessIterator result_last,
    Compare comp,Distance*,T*){
    /*result容器可能小于输入容器。*/
    if(result_first == result_last) return result_last;
    RandomAccessIterator result_iter = result_first;
    while(first != last && result_iter != result_last){
        *result_iter = *first;
        ++first;
        ++result_iter;
    }

    make_heap(result_first,result_iter,comp);
    for(;first != last; ++first){
        if(comp(*first,*result_first))
            adjust_heap(result_first,static_cast<Distance(0),
                static_cast<Distance>(result_last - result_first),(T)(*first),comp);
    }
    sort_heap(result_first,result_iter,comp);
    return result_iter;
}

template<class InputIterator,class RandomAccessIterator,class Compare>
RandomAccessIterator 
partial_sort_copy(InputIterator first,InputIterator last,
    RandomAccessIterator result_first,RandomAccessIterator result_last,Compare comp){
    return __partial_sort_copy(first,last,result_first,result_last,comp,
        distance(result_first),value_type(first));
}

/**************************************************partition******************************************************
*  对区间元素重排，被一元条件判断为true的元素放到前半段，不保证元素的原始相对位置。
*****************************************************************************************************************/
template<class BidirectionalIterator,class Predicate>
BidirectionalIterator partition(BidirectionalIterator first,BidirectionalIterator last,Predicate pred){
    while(true){
        while(true){
            if(first == last) return first;
            else if(pred(*first)) ++first;
            else break;
        }

        --last;
        while(true){
            if(first == last) return first;
            else if(!pred(*last)) --last;
            else break;
        }
        iter_swap(first,last);//交换迭代器所指位置的对象
        ++first;
    }
}

/**********************************************prev_permutation****************************************************
*  将[first,last)的序列变成上一序列，如果有上一序列则返回true，否则返回false。
* 列如： 9 2 3 1 上一序列为 ： 9 2 1 3 返回true; 最开始序列为 1 2 3 9。
*******************************************************************************************************************/
template<class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first,BidirectionalIterator last){
    BidirectionalIterator it = last;
    if(first == last || first == --it) return false;
    for(;;){
        BidirectionalIterator iit = it;
        --it;
        if(*iit < *it){/*如果前一个元素大于后一个元素*/
            BidirectionalIterator jit = last;
            /* 
            *  note: (*--jit < *it)，即从last往前找到第一个不大于it的元素。
            * 那么while循环条件自然就是：！(*--jit < *it),就一直循环。
            */
            while(!(*--jit < *it)){}
            iter_swap(it,jit);
            reverse(iit,last);
            return true;
        }

        if(first == it){
            reverse(first,last);
            break;
        }
    }
    return false;
}

template<class BidirectionalIterator,class Compare>
bool prev_permutation(BidirectionalIterator first,BidirectionalIterator last,Compare comp){
    BidirectionalIterator it = last;
    if(first == last || first == --it) return false;
    for(;;){
        BidirectionalIterator iit = it;
        --it;
        if(comp(*iit,*it)){
            BidirectionalIterator jit = last;
            while(!comp(*--jit,*it)){}
            iter_swap(it,jit);
            reverse(iit,last);
            return true;
        }

        if(first == it){
            reverse(first,last);
            break;
        }
    }
    return false;
}

/**************************************************remove_copy******************************************************
*  移除与value相等的元素，并将结果（结果是不等于value的元素）复制到以result为起始的位置的容器上。
********************************************************************************************************************/
template<class InputIterator,class OutputIterator,class T>
OutputIterator remove_copy(InputIterator first,InputIterator last,OutputIterator result,const T& value){
    for(;first != last; ++first){
        if(*first != value){
            *result = *first;
            ++result;
        }
    }
    return result;
}

/****************************************************remove********************************************************
*  移除与value相等的元素，并不从容器删除这些元素，所以remove,remove_if不适合array。
********************************************************************************************************************/
template<class ForwardIterator,class T>
ForwardIterator remove(ForwardIterator first,ForwardIterator last,const T& value){
    first = find(first,last,value);
    ForwardIterator next = first;
    return last == first ? first : remove_copy(++next,last,first,value);
}


/**************************************************remove_copy_if***************************************************
*  移除区间所有令仿函数pred为true的元素,并将结果（结果是使pred为false的元素）复制到以result开始的容器上。
********************************************************************************************************************/
template<class InputIterator,class OutputIterator,class Predicate>
OutputIterator remove_copy_if(InputIterator first,InputIterator last,
    OutputIterator result,Predicate pred){
    for(;first != last; ++first){
        if(!pred(*first)){
            *result = *first;
            ++result;
        }
    }
    return result;
}

/***********************************************remove_if************************************************
*  移除区间所有令仿函数pred为true的元素。
*********************************************************************************************************/
template<class ForwardIterator,class Predicate>
ForwardIterator remove_if(ForwardIterator first,ForwardIterator last,Predicate pred){
    first = find_if(first,last,pred);
    ForwardIterator next = first;
    return first == last ? first : remove_copy_if(++next,last,first,pred);
}



/*************************************************replace************************************************
 *  将区间所有old_value都以new_value替代。
*********************************************************************************************************/
template<class ForwardIterator,class T>
void replace(ForwardIterator first,ForwardIterator last,const T& old_value,const T& new_value){
    for(;first != last; ++first){
        if(*first == old_value)
            *first = new_value;
    }
}

/**********************************************replace_copy**********************************************
 *  将区间所有old_value都以new_value替代,但将替换后的结果输出到result开始的容器，不改变原来容器。
*********************************************************************************************************/
template<class ForwardIterator,class OutputIterator,class T>
OutputIterator replace_copy(ForwardIterator first,ForwardIterator last,
    OutputIterator result,const T& old_value,const T& new_value){
    for(;first != last; ++first){
        *result = *first == old_value? new_value : *first;
    }
    return result;
}

/************************************************replace_if**********************************************
 *  将所有令pred仿函数为true的元素都以new_value替代。
*********************************************************************************************************/
template<class ForwardIterator,class Predicate,class T>
void replace_if(ForwardIterator first,ForwardIterator last,Predicate pred,const T& new_value){
    for(;first != last; ++first){
        if(pred(*first)) *first = new_value;
    }
}

/*******************************************replace_copy_if**********************************************
 *  将所有令pred仿函数为true的元素都以new_value替代，但将结果输出到result开始的容器，不改变原容器。
*********************************************************************************************************/
template<class ForwardIterator,class OutputIterator,class Predicate,class T>
OutputIterator replace_if(ForwardIterator first,ForwardIterator last,
    OutputIterator result,Predicate pred,const T& new_value){
    for(;first != last; ++first,++result){
        *result = pred(*first)? new_value : *first;
    }
    return result;
}


/***********************************************reverse**************************************************
 *  颠倒序列中的元素。
*********************************************************************************************************/
template<class BidirectionalIterator>
void __reverse(BidirectionalIterator first,BidirectionalIterator last,bidirectional_iterator_tag){
    while(true){
        if(first = last || first == -last)
            return;
        else
            iter_swap(first++,last);
    }
}

template<class RandomAccessIterator>
void __reverse(RandomAccessIterator first,RandomAccessIterator last,random_access_iterator_tag){
    while(first < last){
        iter_swap(first++,--last);
    }
}

template<class RandomAccessIterator>
void __reverse(RandomAccessIterator first,RandomAccessIterator last){
    __reverse(first,last,iterator_category(first));
}

/*********************************************reverse_copy***********************************************
 *  颠倒序列中的元素,但将颠倒结果输入到以result开始的容器。
*********************************************************************************************************/
template<class BidirectionalIterator,class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first,BidirectionalIterator last,OutputIterator result){
    while(first != last){
        --last;
        *result = *last;
        ++result;
    }
    return result;
}

/*************************************************rotate*************************************************
 *  将[first,middle)内的元素和[middle,last)内元素互换，可以交换两个长度不同的区间。
*********************************************************************************************************/
template<class ForwardIterator,class Distance>
void __rotate(ForwardIterator first,ForwardIterator middle,
    ForwardIterator last,Distance*,forward_iterator_tag){
    for(ForwardIterator it = middle; ; ){
        iter_swap(first,it);
        ++first,++it;
        if(first == middle){
            if(it == last) return;/*后半段也交换完成*/
            middle = it;
        }else if(it == last){
            /*这里前半段未交换完毕，但it已到结尾，将it重置到middle。
            * 此时的middle是之前交换.0
            */
            it = middle; 
        }
    }
}

template<class BidirectionalIterator,class Distance>
void __rotate(BidirectionalIterator first,BidirectionalIterator middle,
    BidirectionalIterator last,Distance*,bidirectional_iterator_tag){
    reverse(first,middle);
    reverse(middle,last);
    reverse(first,last);
}

/*最大公因子*/
template<class EuclideanRingElement>
EuclideanRingElement __gcd(EuclideanRingElement m,EuclideanRingElement n){
    while(n != 0){
        EuclideanRingElement t = m % n;
        m = n;
        n = t;
    }
    return m;
}

template<class RandomAccessIterator,class Distance,class T>
void __rotate_cycle(RandomAccessIterator first,RandomAccessIterator last,
    RandomAccessIterator initial,Distance shift,T*){
    T value = *initial;
    RandomAccessIterator ptr1 = initial;
    RandomAccessIterator ptr2 = ptr1 + shift;

    while(ptr2 != initial){
        *ptr1 = *ptr2;
        ptr1 = ptr2;
        if(last - ptr2 > shift)
            ptr2 += shift;
        else
            ptr2 = first + (shift - (last - ptr2));
    }
    *ptr1 = value;
}

template<class RandomAccessIterator,class Distance>
void __rotate(RandomAccessIterator first,RandomAccessIterator middle,
    RandomAccessIterator last,Distance*,random_access_iterator_tag){
    Distance n = __gcd(last -first,middle - first);
    /*前半段长度与整个容器长度的最大公因数n为循环次数。*/
    /*循环次数确实有讲究，如果n随意，那么存在有些元素没有交换。*/
    /*
    *   为什么最大公因数： 前半段长度可分为一段段长为n的一小节，整个容器也可以分为
    * 一段段长度为n的一个小节。那么每段分成n个元素，前半段和整段都可以分最小数量的
    * 段，这样就可以最少移动。
    */
    while(n--){
        /*first + n ? 为什么？ first + n正是第二个n小段开始，跟每个+shift交换。*/
        /*middle - first，偏移量，用来确定交换元素位置。*/
        __rotate_cycle(first,last,first+n,middle - first,value_type(first));
    }
}

/***********************************************rotate_copy************************************************
 *  与rotate相同，但将结果输入到result容器。
 * ********************************************************************************************************/
template<class ForwardIterator,class OutputIterator>
ForwardIterator rotate_copy(ForwardIterator first,ForwardIterator middle,
    ForwardIterator last,OutputIterator result){
    return copy(first,middle,copy(middle,last,result));
}

/***********************************************search****************************************************
 * 在[first1,last1)中查找[first2,last2)首次出现的地方。
 * *******************************************************************************************************/
template<class ForwardIterator1,class ForwardIterator2,class Distance1,class Distance2>
ForwardIterator1 __search(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2,Distance1*,Distance2*){
    Distance1 d1 = distance(first1,last1);
    Distance2 d2 = distance(first2,last2);
    if(d1 < d2) return last1;

    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;
    while(current2 != last2){/*当current2 == last，说明第二序列匹配完毕。*/
        if(*current1 == *current2){
            ++current1;
            ++current2;
        }else{
            if(d1 == d2)
                return last1;
            else{
                current1 = ++first1;
                current2 = first2;
                --d1;/*current1往后移，所以d1长度减一。*/
            }
        }
    }
    return first1;
}

template<class ForwardIterator1,class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2){
    return __search(first1,last1,first2,last2,distance_type(first1),distance_type(first2));
}

/*Compare重载版本*/
template<class ForwardIterator1,class ForwardIterator2,class Compare,class Distance1,class Distance2>
ForwardIterator1 __search(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2,Compare comp,Distance1*,Distance2*){
    Distance1 d1 = distance(first1,last1);
    Distance2 d2 = distance(first2,last2);
    if(d1 < d2) return last1;

    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;
    while(current2 != last2){/*当current2 == last，说明第二序列匹配完毕。*/
        if(comp(*current1,*current2)){
            ++current1;
            ++current2;
        }else{
            if(d1 == d2)
                return last1;
            else{
                current1 = ++first1;
                current2 = first2;
                --d1;/*current1往后移，所以d1长度减一。*/
            }
        }
    }
    return first1;
}

template<class ForwardIterator1,class ForwardIterator2,class Compare>
inline ForwardIterator1 search(ForwardIterator1 first1,ForwardIterator1 last1,
    ForwardIterator2 first2,ForwardIterator2 last2,Compare comp){
    return __search(first1,last1,first2,last2,comp,distance_type(first1),distance_type(first2));
}

/***********************************************search_n************************************************
 * 在[first,last)中查找连续n个value所形成的子序列，返回一个迭代器指向子序列起始位置。
 ******************************************************************************************************/
template<class ForwardIterator,class Count,class T>
ForwardIterator search_n(ForwardIterator first,ForwardIterator last,Count n,const T& value){
    if(n <= 0) return first;
    else{
        first = find(first,last,value);
        while(first != last){
            Count m = n - 1;
            ForwardIterator it = first;
            ++it;
            while(it != last && m != 0 && *it == value){
                ++it,--m;
            }
            if(m == 0){
                return first;
            }else{
                first = find(it,last,value);
            }
        }
    }
    return last;
}

template<class ForwardIterator,class Count,class T,class Compare>
ForwardIterator search_n(ForwardIterator first,ForwardIterator last,
    Count n,const T& value,Compare comp){
    if(n <= 0) return first;
    else{
        first = find(first,last,value);
        while(first != last){
            Count m = n - 1;
            ForwardIterator it = first;
            ++it;
            while(it != last && m != 0 && comp(*it,value)){
                ++it,--m;
            }
            if(m == 0){
                return first;
            }else{
                first = find(it,last,value);
            }
        }
    }
    return last;
}

/***********************************************swap_ranges************************************************
 *     交换[first1,last1)内的元素跟从first2开始、个数相同的元素交换。
 * 交换区间相同，成功返回指向序列2最后一个交换元素的下一个位置。
 * *******************************************************************************************************/
template<class ForwardIterator1,class ForwardIterator2>
ForwardIterator2 
swap_ranges(ForwardIterator1 first1,ForwardIterator1 last1,ForwardIterator2 first2){
    for(;first1 != last1 ; ++first1,++first2){
        iter_swap(first1,first2);
    }
    return first2;
}

/***********************************************transform************************************************
 *  第一版以仿函数op作用于[first,last)中每个元素，并将结果保存至result
 *  第二版以仿函数binary_op，作用于两个序列[first1,last1),[first2,last2)的相同位置。
 * ******************************************************************************************************/
template<class InputIterator,class OutputIterator,class UnaryOperation>
OutputIterator transform(InputIterator first,InputIterator last,
    OutputIterator result,UnaryOperation op){
    for(;first != last; ++first,++result)
        *result = op(*first);
    return result;
}

template<class InputIterator1,class InputIterator2,class OutputIterator,class BinaryOperation>
OutputIterator transform(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2,
    InputIterator2 last2,OutputIterator result,BinaryOperation op){
    for(;first1 != last1; ++first1,++first2,++result)
        *result = op(*first1,*first2);
    return result;
}

/********************************************unique_copy**********************************************
 *  将[first,last)中的元素复制到result，如果元素重复则只复制一次。序列必须有序。
 * ***************************************************************************************************/
/*forward_iterator_tag版本*/
template<class InputIterator,class ForwardIterator>
ForwardIterator __unique_copy(InputIterator first,InputIterator last,
    ForwardIterator result,forward_iterator_tag){
    *result = *first;
    while(++first != last){
        if(*result != *first)
            *++result = *first;
    }
    return ++result;
}

/*output_iterator_tag版本辅助函数，因为output_iterator是只写容器，所以得有类型T中转。*/
template<class InputIterator,class OutputIterator,class T>
OutputIterator __unique_copy_aux(InputIterator first,InputIterator last,
    OutputIterator result,T*){
    T value = *first;
    *result = value;
    while(++first != last){
        if(value != *first){
            *++result = *first;
        }
    }
    return ++result;
}

/*output_iterator_tag版本*/
template<class InputIterator,class OutputIterator>
OutputIterator __unique_copy(InputIterator first,InputIterator last,
    OutputIterator result,output_iterator_tag){
    return __unique_copy_aux(first,last,result,value_type(first));
}

template<class InputIterator,class OutputIterator>
OutputIterator unique_copy(InputIterator first,InputIterator last,OutputIterator result){
    if(first == last)   return result;
    return __unique_copy(first,last,result,iterator_category(first));
}


/*unique_copy使用Compare版本*/
template<class InputIterator,class ForwardIterator,class Compare>
ForwardIterator __unique_copy(InputIterator first,InputIterator last,
    ForwardIterator result,Compare comp,forward_iterator_tag){
    *result = *first;
    while(++first != last){
        if(!comp(*result,*first))
            *++result = *first;
    }
    return ++result;
}

template<class InputIterator,class OutputIterator,class Compare,class T>
OutputIterator __unique_copy_aux(InputIterator first,InputIterator last,
    OutputIterator result,Compare comp,T*){
    T value = *first;
    *result = value;
    while(++first != last){
        if(!comp(value,*first)){
            *++result = *first;
        }
    }
    return ++result;
}

template<class InputIterator,class OutputIterator,class Compare>
OutputIterator __unique_copy(InputIterator first,InputIterator last,
    OutputIterator result,Compare comp,output_iterator_tag){
    return __unique_copy_aux(first,last,result,comp,value_type(first));
}

template<class InputIterator,class OutputIterator,class Compare>
OutputIterator unique_copy(InputIterator first,InputIterator last,
    OutputIterator result,Compare comp){
    if(first == last)   return result;
    return __unique_copy(first,last,result,comp,iterator_category(first));
}

/***********************************************unique************************************************
 *  移除[first,last)中重复的元素，序列必须有序，它并不删除重复元素。
 * ***************************************************************************************************/
template<class ForwardIterator>
ForwardIterator unique(ForwardIterator first,ForwardIterator last){
    first = adjacent_find(first,last);
    return unique_copy(first,last,first);
}

template<class ForwardIterator,class Compare>
ForwardIterator unique(ForwardIterator first,ForwardIterator last,Compare comp){
    first = adjacent_find(first,last,comp);
    return unique_copy(first,last,first,comp);
}

/***********************************************sort************************************************
 * 将区间[first,last)进行排序，默认以递增方式排序。
 * *************************************************************************************************/
template<class RandomAccessIterator>
inline void sort(RandomAccessIterator first,RandomAccessIterator last){
    if(first == last) return;
    __introsort_loop(first,last,value_type(first),__lg(last - first) * 2);
    __final_insertion_sort(first,last);
}

/*控制分割恶化的情况。*/
template<class Size>
inline Size __lg(Size n){ /*得到使得 2^result <= n 时，result的最大值。*/
    Size result;
    for(result = 0; n > 1 ; n >> 1) ++result;
    return result;
}

const int __SECTIONSIZE = 16;

/*内省式排序，先进行quick_sort,当分割行为出现恶化倾向时，改用heap_sort。*/
template<class RandomAccessIterator,class T,class Size>
void __introsort_loop(RandomAccessIterator first,RandomAccessIterator last,
    T*,Size depth_limit){
    /*给定区间或递归划分后的区间大于16，进行递归划分。*/
    while(last - first > __SECTIONSIZE){
        /*限制了递归层数，到达最大递归层数，不再递归，而是立即堆排序。*/
        if(depth_limit == 0){
            partial_sort(first,last,last);
            return;
        }

        --depth_limit;
        /*选出最后一个元素进行归位，并返回这个元素归位后的位置。*/
        RandomAccessIterator cut = __unguarded_partition(first,last,
            T(median(*first,*(first + (last - first)/2),*(last-1))));
        
        /*对右半区递归。*/
        __introsort_loop(cut,last,value_type(first),depth_limit);
        /*递归返回后对左半区进行递归。*/
        last = cut;
    }
}

template<class RandomAccessIterator,class T>
RandomAccessIterator 
__unguarded_partition(RandomAccessIterator first,RandomAccessIterator last,T pivot){
    while(true){
        while(*first < pivot) ++first; /*找到第一个大于pivot的元素*/
        --last;
        while(*pivot < *last) --last; /*从右往左找到第一个小于last的元素。*/
        if(!(first < last)) return first; /*交错，循环结束。*/
        iter_swap(first,last); /*交换两个位置。*/
        ++first;
    }
}

template<class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first,RandomAccessIterator last){
    if(last - first > __SECTIONSIZE){
        /*前16个元素前部分用插入排序*/
        __insertion_sort(first,first + __SECTIONSIZE);
        /*16个元素之后的使用插入排序*/
        __unguarded_insertion_sort(first + __SECTIONSIZE,last);
    }else/*小于__SECTIONSIZE直接使用插入排序。*/
        __insertion_sort(first,last);
}

/*插入排序*/
template<class RandomAccessIterator>
void __insertion_sort(RandomAccessIterator first,RandomAccessIterator last){
    if(first == last) return ;
    for(RandomAccessIterator it = first + 1; it != last; ++it)
        __linear_insert(first,it,value_type(first));/*对子区间[first,it)插入元素。*/
}

template<class RandomAccessIterator,class T>
inline void __linear_insert(RandomAccessIterator first,RandomAccessIterator last,T*){
    T value = *last;
    if(value < *first){
        copy_backward(first,last,last+1);
        *first = value;
    }else  /*前一个if条件不成立，保证了value一定不是第一个元素，那么下面这个函数可以调用。*/
        __unguarded_linear_insert(last,value);
}

template<class RandomAccessIterator,class T>
void __unguarded_linear_insert(RandomAccessIterator last,T value){
    RandomAccessIterator next = last;
    --next;
    while(value < *next){
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}


template<class RandomAccessIterator>
inline void 
__unguarded_insertion_sort(RandomAccessIterator first,RandomAccessIterator last){
    __unguarded_insertion_sort_aux(first,last,value_type(first));
}

template<class RandomAccessIterator,class T>
void __unguarded_insertion_sort_aux(RandomAccessIterator first,
    RandomAccessIterator last,T*){
    for(RandomAccessIterator it = first; it != last; ++it)
        __unguarded_linear_insert(it,T(*it));
}

/*使用仿函数comp版本。*/
template<class RandomAccessIterator,class Compare>
inline void sort(RandomAccessIterator first,RandomAccessIterator last,Compare comp){
    if(first == last) return;
    __introsort_loop(first,last,value_type(first),__lg(last - first) * 2,comp);
    __final_insertion_sort(first,last,comp);
}

/*内省式排序，先进行quick_sort,当分割行为出现恶化倾向时，改用heap_sort。*/
template<class RandomAccessIterator,class T,class Size,class Compare>
void __introsort_loop(RandomAccessIterator first,RandomAccessIterator last,
    T*,Size depth_limit,Compare comp){
    while(last - first > __SECTIONSIZE){
        if(depth_limit == 0){
            partial_sort(first,last,last,comp);
            return;
        }

        --depth_limit;
        RandomAccessIterator cut = __unguarded_partition(first,last,
            T(median(*first,*(first + (last - first)/2),*(last-1))),comp);
        __introsort_loop(cut,last,value_type(first),depth_limit);
        last = cut;
    }
}

template<class RandomAccessIterator,class T,class Compare>
RandomAccessIterator 
__unguarded_partition(RandomAccessIterator first,RandomAccessIterator last,
    T pivot,Compare comp){
    while(true){
        while(comp(*first,pivot)) ++first; /*找到第一个大于pivot的元素*/
        --last;
        while(comp(*pivot,*last)) --last; /*从右往左找到第一个小于last的元素。*/
        if(!(first < last)) return first; /*交错，循环结束。*/
        iter_swap(first,last); /*交换两个位置。*/
        ++first;
    }
}

template<class RandomAccessIterator,class Compare>
void __final_insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last,Compare comp){
    if(last - first > __SECTIONSIZE){ /*大于__SECTIONSIZE前部分用插入排序*/
        __insertion_sort(first,first + __SECTIONSIZE,comp);
        __unguarded_insertion_sort(first + __SECTIONSIZE,last,comp);
    }else/*小于__SECTIONSIZE直接使用插入排序。*/
        __insertion_sort(first,last,comp);
}

/*插入排序*/
template<class RandomAccessIterator,class Compare>
void __insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last,Compare comp){
    if(first == last) return ;
    for(RandomAccessIterator it = first + 1; it != last; ++it)
        __linear_insert(first,it,value_type(first),comp);/*对子区间[first,it)插入元素。*/
}

template<class RandomAccessIterator,class T,class Compare>
inline void __linear_insert(RandomAccessIterator first,
    RandomAccessIterator last,T*,Compare comp){
    T value = *last;
    if(comp(value,*first)){
        copy_backward(first,last,last+1);
        *first = value;
    }else  /*前一个if条件不成立，保证了value一定不是第一个元素，那么下面这个函数可以调用。*/
        __unguarded_linear_insert(last,value,comp);
}

template<class RandomAccessIterator,class T,class Compare>
void __unguarded_linear_insert(RandomAccessIterator last,T value,Compare comp){
    RandomAccessIterator next = last;
    --next;
    while(comp(value,*next)){
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}


template<class RandomAccessIterator,class Compare>
inline void 
__unguarded_insertion_sort(RandomAccessIterator first,RandomAccessIterator last,Compare comp){
    __unguarded_insertion_sort_aux(first,last,value_type(first),comp);
}

template<class RandomAccessIterator,class T,class Compare>
void __unguarded_insertion_sort_aux(RandomAccessIterator first,
    RandomAccessIterator last,T*,Compare comp){
    for(RandomAccessIterator it = first; it != last; ++it)
        __unguarded_linear_insert(it,T(*it),comp);
}

/***********************************************nth_element************************************************
 * 对序列进行重排，使得所有小于第n个元素出现在它的前面，大于它的出现在它的后面。
 **********************************************************************************************************/
template<class RandomAccessIterator>
inline void nth_element(RandomAccessIterator first,RandomAccessIterator nth,
    RandomAccessIterator last){
    __nth_element(first,nth,last,value_type(first));
}

template<class RandomAccessIterator,class T>
inline void __nth_element(RandomAccessIterator first,RandomAccessIterator nth,
    RandomAccessIterator last,T*){
    while(last - first > 3){
        /*
        *   每次取中间值，使得所有小于中间值放入左边，大于中间值放入右边。
        * 为什么不直接取第n个位置元素呢？因为第n位置元素不一定就是排序后
        * 的第n大的元素。
        */
        RandomAccessIterator cut = __unguarded_partition(first,last,
        T(median(*first,*(first + (last - first)/2),*(last - 1))));
        if(cut <= nth) /*cut是已归位的元素位置，任何cut右边元素都大于等于*cut。所以第n个元素也一定在右边。*/
            first = cut;
        else
            last = cut;
    }
    __insertion_sort(first,last);
}

template<class RandomAccessIterator,class Compare>
inline void nth_element(RandomAccessIterator first,RandomAccessIterator nth,
    RandomAccessIterator last,Compare comp){
    __nth_element(first,nth,last,value_type(first),comp);
}

template<class RandomAccessIterator,class T,class Compare>
inline void __nth_element(RandomAccessIterator first,RandomAccessIterator nth,
    RandomAccessIterator last,T*,Compare comp){
    while(last - first > 3){
        /*
        *   每次取中间值，使得所有小于中间值放入左边，大于中间值放入右边。
        * 为什么不直接取第n个位置元素呢？因为第n位置元素不一定就是排序后
        * 的第n大的元素。
        */
        RandomAccessIterator cut = __unguarded_partition(first,last,
        T(median(*first,*(first + (last - first)/2),*(last - 1))),comp);
        if(cut <= nth) /*cut是已归位的元素位置，任何cut右边元素都大于等于*cut。所以第n个元素也一定在右边。*/
            first = cut;
        else
            last = cut;
    }
    __insertion_sort(first,last,comp);
}

/***********************************************random_shuffle***********************************************
 * 将[first,last)内的元素随机重排。
 ************************************************************************************************************/
template<class RandomAccessIterator>
inline void random_shuffle(RandomAccessIterator first,RandomAccessIterator last){
    __random_shuffle(first,last,distance_type(first));
}

template<class RandomAccessIterator,class Distance>
void __random_shuffle(RandomAccessIterator first,RandomAccessIterator last,Distance*){
    if(first == last) return ;
    srand((unsigned)time(NULL));
    /*将当前it位置元素跟[first,it]的任意元素进行交换。*/
    for(RandomAccessIterator it = first + 1; it != last; ++it)
        iter_swap(it,first+Distance(rand() % ((it -first) + 1)));
}

template<class RandomAccessIterator,class RandomNumberGenerator>
inline void random_shuffle(RandomAccessIterator first,
    RandomAccessIterator last,RandomNumberGenerator& rand){
    if(first == last) return ;
    for(RandomAccessIterator it = first + 1; it != last; ++it)
        iter_swap(it,first+rand((it - first) + 1));
}
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
}// namespace ZMJ
#endif// !__ALGO_H__