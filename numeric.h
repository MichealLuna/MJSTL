#ifndef __NUMERIC_H__
#define __NUMERIC_H__

#include "iterator.h"

namespace ZMJ{

/*accumulate:累加*/
template<class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init){
    for(; first != last; ++first)
        init += *first;
    return init;
}

template<class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op){
    for(; first != last; ++first)
        init = binary_op(init, *first);
    return init;
}


/*adjacent_difference:相邻元素之差输入到result容器*/
template<class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result){
    if(first == last) return result;
    *result = *first;
    typedef typename iterator_traits<InputIterator>::value_type value_type;
    value_type value = *first;
    while(++first != last){
        value_type tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }
    return ++result;
}

template<class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator
adjacent_difference(InputIterator first, InputIterator last, 
    OutputIterator result, BinaryOperation binary_op){
    if(first == last) return result;
    *result = *first;
    typedef typename iterator_traits<InputIterator>::value_type value_type;
    value_type value = *first;
    while(++first != last){
        value_type tmp = *first;
        *++result = binary_op(tmp,value);
        value = tmp;
    }
    return ++result;
}


/*inner_product: 两个容器的元素分别求內积*/
template<class InputIterator1,class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, InputIterator2 last2,T init){
    for(;first1 != last1; ++first1, ++first2)
        init = init + (*first1 * *first2);
    return init;
}

/*这个版本：需要两个操作 1）怎么做內积op1； 2） 怎么放入累加op2。*/
template<class InputIterator1,class InputIterator2,
    class T,class BinaryOperation1,class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, InputIterator2 last2,T init,
    BinaryOperation1 binary_op1, BinaryOperation2 binary_op2){
    for(;first1 != last1; ++first1, ++first2)
        init = binary_op1(init, binary_op2(*first1, *first2));
    return init;
}


/*partial_sum:局部求和,这其实也是前缀和。*/
template<class InputIterator,class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result){
    if(first == last) return result;
    *result = *first;
    typedef typename iterator_traits<InputIterator>::value_type value_type;
    value_type value = *first;
    while(++first != last){
        value = value + *first;
        *++result = value;
    }
    return ++result;
}

template<class InputIterator,class OutputIterator,class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
    OutputIterator result, BinaryOperation binary_op){
    if(first == last) return result;
    *result = *first;
    typedef typename iterator_traits<InputIterator>::value_type value_type;
    value_type value = *first;
    while(++first != last){
        value = binary_op(value, *first);
        *++result = value;
    }
    return ++result;
}

}//namespace ZMJ

#endif//numeric.h