#ifndef __SET_ALGO_H__
#define __SET_ALGO_H__

#include "iterator.h"
#include "algobase.h"
#include <algorithm>
namespace ZMJ{

/*set即集合的四种算法： union并集； intersection交集； difference差集； symmetric difference对称差集*/
/*接收set、multiset容器作为输入区间*/

/*********************************************set_union**********************************************/
template<class InputIterator1,class InputIterator2,class OutputIterator>
OutputIterator set_union(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result){
    /*小的放入result，巧妙，如果碰到相等的只放入其中1个。*/
    while(first1 != last1 && first2 != last2){
        if(*first1 < *first2){
            *result = *first1;
            ++first1;
        }else if(*first2 < *first1){
            *result = *first2;
            ++first2;
        }else{
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    /*剩下的一定是可以全部拷贝到result的。这里必有其中copy是没有做任何操作的。*/
    return copy(first1,last1,copy(first2,last2,result));
}

/*********************************************set_intersection**********************************************/
template<class InputIterator1,class InputIterator2,class OutputIterator>
inline OutputIterator
set_intersection(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result){
    while(first1 != last1 && first2 != last2){
        if(*first1 < *first2) ++first1;
        else if(*first2 < *first1) ++first2;
        else{
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
        /*return result; 在这里返回？不对吧！*/
    }
    return result;
}

template<class InputIterator1,class InputIterator2,class OutputIterator,class Compare>
inline OutputIterator
set_intersection(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result,Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first1,*first2)) ++first1;
        else if(comp(*first2,*first1)) ++first2;
        else{
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

/*********************************************set_difference**********************************************/
/*计算集合S1 - 集合S2差集，返回result为结果的尾部。这个函数前提是容器是有序的。*/
template<class InputIterator1,class InputIterator2,class OutputIterator>
inline OutputIterator
set_difference(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result){
    while(first1 != last1 && first2 != last2){
        if(*first1 < *first2){
            *result = *first1;
            ++first1;
            ++result;
        }else if(*first2 < *first1){
            ++first2;
        }else{/*差集：S1 - S2集合，也就是排除S1中跟S2的相等的元素。*/
            ++first1;
            ++first2;
        }
    }
    /*遍历整个S1集合或者S2集合后，S1集合剩余的元素一定就是S2没有的，可以全部加入result*/
    return copy(first1,last1,result);
}

template<class InputIterator1,class InputIterator2,class OutputIterator,class Compare>
inline OutputIterator
set_difference(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result,Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first1,*first2)){
            *result = *first1;
            ++first1;
            ++result;
        }else if(comp(*first2,*first1)){
            ++first2;
        }else{
            ++first1;
            ++first2;
        }
    }
    return copy(first1,last1,result);
}

/*********************************************set_symmetric_difference**********************************************/
template<class InputIterator1,class InputIterator2,class OutputIterator>
inline OutputIterator
set_symmetric_difference(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result){
    while(first1 != last1 && first2 != last2){
        if(*first1 < *first2){
            *result = *first1;
            ++first1;
            ++result;
        }else if(*first2 < *first1){
            *result = *first2;
            ++first2;
            ++result;
        }else{
            ++first1;
            ++first2;
        }
    }
    return copy(first1,last1,copy(first2,last2,result));
}

template<class InputIterator1,class InputIterator2,class OutputIterator,class Compare>
inline OutputIterator
set_symmetric_difference(InputIterator1 first1,InputIterator1 last1,
    InputIterator2 first2,InputIterator2 last2,OutputIterator result,Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first1,*first2)){
            *result = *first1;
            ++first1;
            ++result;
        }else if(comp(*first2,*first1)){
            *result = *first2;
            ++first2;
            ++result;
        }else{
            ++first1;
            ++first2;
        }
    }
    return copy(first1,last1,copy(first2,last2,result));
}

}//namespace ZMJ

#endif//set_algo.h