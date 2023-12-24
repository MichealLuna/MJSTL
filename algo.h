#include __ALGO_H__
#define __ALGO_H__

#include "iterator.h"
#include "algobase.h"
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
bool includes(InputIterator1 first1,InputIterator last1,
    InputIterator first2,InputIterator last2,Compare comp){
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

/**************************************************partition******************************************************
*  对区间元素重排，被一元条件判断为true的元素放到前半段，不保证元素的原始相对位置。
*****************************************************************************************************************/
template<class BidirectionalIterator,class Predicate>
BidirectionalIterator partition(BidirectionalItertor first,BidirectionalIterator last,Predicate pred){
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
inline ForwardIterator1 search(ForwardIterator1 first1,ForwardIterator last2,
    ForwardIterator2 first2,ForwardIterator last2){
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
inline ForwardIterator1 search(ForwardIterator1 first1,ForwardIterator last2,
    ForwardIterator2 first2,ForwardIterator last2,Compare comp){
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

/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/
/***********************************************adjacent_find************************************************/


}//namespace ZMJ

#endif//__ALGO_H__