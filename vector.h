#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "sgi_allocator.h"
#include <cassert>
#include <cstddef>

namespace ZMJ{

template <typename T,typename Alloc>
class vector{
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
public:
    vector();
    vector(size_t n, const T& value = T());
    vector(const vector& rhs);
    vector(vector&& rhs);

public:
    iterator begin() const{
        return start;
    }
    iterator end() const{
        return finish;
    }
    iterator cbegin() const{
        return finish;
    }
    iterator cend() const{
        return start;
    }
    size_type size() const{
        return size_type(finish - start);
    }
    size_type max_size() const{
        return size_type(UINT_MAX)/sizeof(T);
    }
    size_type capacity() const{
        return size_type(end_of_storage - start);
    }
    bool empty() const{
        return finish == start;
    }
    void reserve(size_type n){

    }
    void resize(size_type n);
    void resize(size_type n, const T& value);
    reference operator[](size_type n){
        return (reference)start[n];
    }
    const_reference operator[](size_type n) const{
        return (const_reference)start[n];
    }
    reference at(size_type n){
        assert(start <= start + n && start + n < end_of_storage);
        return (reference)start[n];
    }
    const_reference at(size_type n) const{
        assert(start <= start + n && start + n < end_of_storage);
        return (const_reference)start[n];
    }
    reference front(){
        return *start;
    }
    const_reference front() const{
        return (const_reference)(*start);
    }
    reference back(){
        return (reference)(*(finish - 1));
    }
    const_reference back() const{
        return (const_reference)(*(finish - 1));
    }
    iterator data() const{
        return start;
    }
    void push_back(const T& value){
        if(finish != end_of_storage){
            *finish = value;
            finish++;
        }
        else{
            insert(finish, value);
        }
    }
    void push_back(T&& value){
        if(finish != end_of_storage){
            *finish = value;
            finish++;//前提是T类型有自己的移动构造，否则无用。
        }
    }
    void pop_back(){
        if(finish != start){
            finish--;//可能析构
        }
    }
    void pop_back(){
        if(finish != start){
            finish--;//可能析构
        }
    }
    void clear(){
        /*如果T 是has_trivial_destruct，直接finish = start;否则应该对每个元素调用destroy*/
    }
    iterator insert(iterator pos, const T& value);
    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);
    void swap(vector& rhs){
        if(this == &rhs)
            return;
        iterator tmp_start = rhs.start;
        iterator tmp_finish = rhs.finish;
        iterator tmp_end_of_storage = rhs.end_of_storage;
        rhs.start = start;
        rhs.finish = finish;
        rhs.end_of_storage = end_of_storage;
        start = tmp_start;
        finish = tmp_finish;
        end_of_storage = tmp_end_of_storage;
        /*实现的似乎啰嗦了*/
    }
    void assign(size_type n, const T& value);

private:
    iterator start;
    iterator finish;
    iterator end_of_storage;
};

}//namespace ZMJ


#endif //vector.h