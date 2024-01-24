#ifndef __HEAP_ALGO_H__
#define __HEAP_ALGO_H__

#include "iterator.h"
namespace mjstl{

/* heap算法： push_heap , pop_heap , sort_heap , make_heap */
/*使用array，vector容器作为heap的容器。*/

/*************************************************push_heap**************************************************/

/*根据给定位置holeIndex，做上虑操作。直到holeIndex的父节点大于value停止（大根堆）。*/
template<class RandomAccessIterator,class Distance,class T>
void push_heap_aux(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value){
    Distance parent = (holeIndex-1)/2;
    /*大根堆： 从末尾往上调整*/
    while(holeIndex > topIndex && *(first+parent) < value){
        /*把当前父节点位置小元素下放。不用担心最开始的holdeIndex位置元素被覆盖，因为有副本value在。*/
        *(first+holeIndex) = *(first+parent);
        holeIndex = parent; /*上滤*/
        parent = (holeIndex-1)/2;/*重新给定holeIndex的父节点。*/
    }
    *(first+holeIndex) = value;
}

/*将容器最后一个元素作为参数传入push_head_aux，使得最后元素不会在上率时被覆盖，因为参数传递产生了副本。*/
template<class RandomAccessIterator, class Distance>
inline void push_heap_d(RandomAccessIterator first, RandomAccessIterator last, Distance*){
    push_head_aux(first,Distance(last-first - 1),Distance(0),*(last-1));
}

/*需要push的元素已经在容器最尾端！*/
template<class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last){
    push_heap_d(first,last,distance_type(first));
}


/*自定义比较函数*/
template<class RandomAccessIterator, class Distance, class T, class Compare>
void push_heap_aux(RandomAccessIterator first, Distance holeIndex, 
    Distance topIndex, T value, Compare comp){
    Distance parent = (holeIndex-1)/2;
    while(holeIndex > topIndex && comp(*(first+parent),value)){
        *(first+holeIndex) = *(first+parent);
        holeIndex = parent;
        parent = (holeIndex-1)/2;
    }
    *(first+holeIndex) = value;
}

template<class RandomAccessIterator,class Compare,class Distance>
inline void push_heap_d(RandomAccessIterator first,RandomAccessIterator last,
    Distance*,Compare cmp){
    push_heap_aux(first,Distance(last - first - 1),Distance(0),*(last-1),cmp);
}

template<class RandomAccessIterator,class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    push_heap_d(first,last,distance_type(first),comp);
}


/*************************************************pop_heap**************************************************/
/* [first,last) 表示容器的首尾,将会把heap根元素放入尾部，并调整heap*/

template<class RandomAccessIterator, class T, class Distance>
void adjust_heap(RandomAccessIterator first, Distance holeIndex,Distance len,T value){
    auto topIndex = holeIndex;
    auto rchild = 2 * holeIndex + 2;
    /*这里递归地从holdeIndex的左右子节点中找到较大的一方成为父节点。*/
    while(rchild < len){
        if(*(first + rchild) < *(first + rchild - 1))
            --rchild;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2 * holeIndex + 2; 
    }

    if(rchild == len){
        /*  rchild正好等于len，意味着无右子节点，那么则一定存在左子节点。
        * 因为因为堆是一个满二叉树，如果rchild刚好不存在，那么rchild刚好
        * 位于数组结尾标志位，那么right-1就一定就是左节点。
        */
        *(first + holeIndex) = *(first + rchild - 1);
        holeIndex = rchild - 1;
    }
    /*从holeIndex开始，又向上调整，找到value位置，并放进去。*/
    push_heap_aux(first,holeIndex,topIndex,value);
}

template<class RandomAccessIterator, class T, class Distance>
void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
    RandomAccessIterator result,T value,Distance*){
    *result = *first;/*尾值被覆盖，难道不是跟尾值交换？并不会，原尾值被作为value参数传递进来！*/
    adjust_heap(first,Distance(0),Distance(last - first),value);
}

template<class RandomAccessIterator, class T>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last, T*){
    pop_heap_aux(first,last-1,last-1,*(last-1),distance_type(first));
}

template<class RandomAccessIterator, class T, class Distance,class Compare>
void adjust_heap(RandomAccessIterator first, Distance holeIndex,
    Distance len, T value,Compare comp){
    Distance topIndex = holeIndex;
    Distance rchild = 2 * holeIndex + 2;
    while(rchild < len){
        if(comp(*(first + rchild),*(first + rchild - 1))) rchild--;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2 * (rchild + 1);
    }
    if(rchild == len){
        *(first + holeIndex) = *(first + rchild - 1);
        holeIndex = rchild - 1;
    }
    push_heap_aux(first,holeIndex,topIndex,value,comp);
}

template<class RandomAccessIterator,class T,class Distance,class Compare>
void pop_head_aux(RandomAccessIterator first, RandomAccessIterator last,
    RandomAccessIterator result, T value, Distance*,Compare comp){
    *result = *first;
    adjust_heap(first,(Distance)0,last - first,value,comp);
}

template<class RandomAccessIterator,class Compare>
void pop_head(RandomAccessIterator first,RandomAccessIterator last,Compare comp){
    pop_head_aux(first,last-1,last-1,*(last-1),distance_type(first),comp);
}

/*************************************************sort_heap**************************************************/
/*前提条件： [first,last) 符合堆序性。*/
template<class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last){
    /*每次元素出栈后都放到了队尾，然后last-1，相当于last维护无序区，而last后面是有序区*/
    while(last - first > 1){
        pop_head(first,last--);
    }
}

template<class RandomAccessIterator,class Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last,Compare comp){
    while(last - first > 1){
        pop_head(first,last--,comp);
    }
}

/*************************************************make_heap**************************************************/
template<class RandomAccessIterator,class Distance>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last,Distance*){
    if(last - first < 2)
        return;
    auto len = last - first;
    auto holeIndex = (len-2)/2;/*最后一个非叶子节点开始往堆顶调整元素，使之满足堆序性。*/
    while(true){
        /*调整容器区间[holeIndex,len)的堆序性，自底向上。*/
        adjust_heap(first,holeIndex,len,*(first+holeIndex));
        if(holeIndex == 0)
            return;
        holeIndex--;
    }
}

template<class RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last){
    make_heap_aux(first,last,distance_type(first));
}

template<class RandomAccessIterator,class Distance,class Compare>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last,Distance*,Compare comp){
    if(last - first < 2)
        return;
    auto len = last - first;
    auto holeIndex = (len-2)/2;
    while(true){
        adjust_heap(first,holeIndex,len,*(first+holeIndex),comp);
        if(holeIndex == 0)
            return;
        holeIndex--;
    }
}

template<class RandomAccessIterator,class Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last,Compare comp){
    make_heap_aux(first,last,distance_type(first),comp);
}

}//namespace mjstl

#endif//__HEAP_ALGO_H__