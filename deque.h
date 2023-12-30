#ifndef __DEQUE_H__
#define __DEQUE_H__

#include "iterator.h"
#include "reverse_iterator.h"
#include "memory.h"

namespace ZMJ
{
    /*
    *   决定缓冲区大小：
    *   @n: 多少个元素。
    *   @sz：每个元素大小。
    *   如果n为0时，并且每个元素大小小于512字节，则缓冲区大小不小于1个的元素空间，
    *  如果每个元素大小大于等于512字节，缓冲区只给定1个元素空间。
    */
    inline size_t __deque_buf_size(size_t n,size_t sz){
        return n != 0 ? n : (sz < 512 ? size_t(512/sz) : size_t(1));
    }

    template<class T,class Ref,class Ptr,size_t BufSize>
    struct __deque_iterator : public iterator<random_access_iterator_tag,T>{
        typedef __queue_iterator<T,T&,T*>               iterator;
        typedef __queue_iterator<T,const T&,const T*>   const_iterator;
        static size_t buffer_size() { return __deque_buf_size(BufSize,sizeof(T));}

        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef size_t size_type;
        typedef T** map_pointer;

        typedef __deque_iterator self;

        T* cur;     /*指向缓冲区的当前元素。*/
        T* first;   /*指向缓冲区的头部。*/
        T* last;    /*指向缓冲区的尾部。*/
        map_pointer node;

        /*constructor*/
        __deque_iterator():cur(0),first(0),last(0),node(0){}
        __deque_iterator(T* x,map_pointer y):cur(x),first(*y),
            last(*y + buffer_size()),node(y);
        __deque_iterator(const iterator& x):cur(x.cur),
            first(x.first),last(x.last),node(x.node){}

        void set_node(map_pointer new_node){
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }

        reference operator*() const { return *cur;}
        pointer operator->() const { return &(operator*());}
        difference_type operator-(const self& x) const{
            /*x在前面，因此还得加上x的cur ~ x的last之间的距离。*/
            return difference_type(buffer_size()) * (node - x.node) + 
                (cur - first) + (x.last - x.cur);
        }

        self& operator++(){
            ++cur;
            if(last == cur){
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int){
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            --cur;
            if(first = cur){
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int){
            self tmp = *this;
            --*this;
            return tmp;
        }

        self& operator+=(difference_type n){
            difference_type offset = n + (cur - first);
            if(offset >= 0 && offset < difference_type(buffer_size())){
                cur += n;
            }else{
                /*      offset为负的情况，说明从当前块first位置往前面的块偏移，
                *   因此new_node一开始就应该是-1，而-offset是为了变成正数好计算
                *   有多少个偏移块，-offset-1，是因为从当前块first开始偏移的，所以
                *   应该多偏移-1。
                */
                difference_type new_node = offset > 0 ? 
                    difference_type(offset/buffer_size()):
                    -difference_type((-offset - 1) / buffer_size()) - 1;
                set_node(node + new_node);
                cur = offset - difference_type(buffer_size() * new_node);
            }
            *this;
        }

        self& operator-=(difference_type n){
            return *this += -n;
        }

        self operator-(difference_type n) const{
            self tmp = *this;
            return tmp -= n;
        }

        self operator+(difference_type n) const{
            self tmp = *this;
            return tmp += n;
        }

        reference operator[](difference_type n) const{
            return *(*this + n);
        }

        bool operator==(const self& x) const{
            return cur == x.cur;
        }

        bool operator!=(const self& x) const{
            return !(*this == x);
        }

        bool operator<(const self& x) const{
            return node == x.node ? (cur < x.cur) : (node < x.node);
        }

        bool operator>(const self& x) const{
            return x < *this;
        }

        bool operator<=(const self& x) const{
            return !(x < *this);
        }

        bool operator>=(const self& x) const{
            return !(*this < x);
        }
    };


} // namespace ZMJ
#endif// !__DEQUE_H__