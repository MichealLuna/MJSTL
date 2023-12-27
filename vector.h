#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "iterator.h"
#include "reverse_iterator.h"
#include "sgi_allocator.h"
#include "uninitialized.h"
#include "algobase.h"

namespace ZMJ{

template <typename T,typename Alloc = simple_alloc<T>>
class vector{
public:
    typedef T                                   value_type;
    typedef Alloc                               allocate_type;
    typedef value_type*                         pointer;
    typedef const value_type*                   const_pointer;
    typedef value_type&                         reference;
    typedef const value_type&                   const_reference;
    typedef size_t                              size_type;
    typedef ptrdiff_t                           difference_type;

    /*iterator_type*/
    typedef value_type*                         iterator;
    typedef const value_type*                   const_iterator;
    typedef reverse_iterator<const_iterator>    const_reverse_iterator;
    typedef reverse_iterator<iterator>          reverse_iterator;
protected:
    typedef Alloc                               data_allocator;

protected:
    iterator start;
    iterator finish;
    iterator end_of_storage;

public:
    /*construct,assignment,destruct*/
    vector():start(nullptr),finish(nullptr),end_of_storage(nullptr){}
    explicit vector(size_type n){ allocate_and_fill(n,T()); }
    vector(size_type n, const T& value){ allocate_and_fill(n,value); }
    template<class InputIterator>
    vector(InputIterator first,InputIterator last);

    /*copy construct*/
    vector(const vector<T,Alloc>& x);
    vector(vector<T,Alloc>&& x);

    /*assignment operator*/
    vector<T,Alloc>& operator=(const vector<T,Alloc>& x);
    vector<T,Alloc>& operator=(vector<T,Alloc>&& x);

    /*destrust*/
    ~vector(){ destory_and_deallocate(); }
public:
    /*about iterator*/
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const{ return finish; }
    reverse_iterator rbegin() { return reverse_iterator(end());}
    const_reverse_iterator rbegin() const{ return const_reverse_iterator(end());}
    reverse_iterator rend() { return reverse_iterator(begin());}
    const_reverse_iterator rend() const{ return const_reverse_iterator(begin());}

    /*about containter*/
    size_type size() const{ return size_type(end() - begin());}
    size_type max_size() const{ return size_type(-1)/sizeof(T);}
    size_type capacity() const{ return size_type(end_of_storage - begin());}
    bool empty() const{ return begin() == end();}
    void resize(size_type new_size,const T& value);
    void resize(size_type new_size){ return resize(new_size,T());}

    /*access container*/
    reference operator[](size_type n){ return *(begin() + n);}
    const_reference operator[](size_type n) const{ return (const_reference)start[n];}
    reference at(size_type n){
        assert(start <= start + n && start + n < end_of_storage);
        return (*this)[n];
    }
    const_reference at(size_type n) const{
        assert(start <= start + n && start + n < end_of_storage);
        return (*this)[n];
    }
    reference front(){ return *begin();}
    const_reference front() const{ return (const_reference)(*begin());}
    reference back(){ return *(end() - 1);}
    const_reference back() const{ return (const_reference)(*(end() - 1));}
    iterator data() const{ return start;}

    /*modify container*/
    void push_back(const T& value);
    void pop_back();
    iterator earse(iterator position);
    iterator earse(iterator first,iterator last);
    void clear();
    void insert(iterator pos, size_type n,const T& value);
    inline void swap(vector<T,Alloc>& rhs);
    inline void swap(vector<T,Alloc>& x,vector<T,Alloc>& y);

    /*about allocator*/
    allocate_type get_allocator(){ return allocate_type();}
protected:
    template<class Integer>
    void vector_construct(Integer n,Integer value,__true_type);
    template<class InputIterator>
    void vector_construct(InputIterator first,InputIterator last,__false_type);
    void destory_and_deallocate();
    void allocate_and_fill(size_type n,const T& value);
    iterator allocate_and_copy(iterator first,iterator last);
    void insert_aux(iterator position,const T& value);
public:
};

template<class T,class Alloc>
template<class InputIterator>
vector<T,Alloc>::vector(InputIterator first,InputIterator last){
    typename __is_integer<InputIterator>::is_integer integer;
    vector_construct(first,last,integer());
}

template<class T,class Alloc>
template<class Integer>
void vector<T,Alloc>::vector_construct(Integer n,Integer value,__true_type){
    allocate_and_fill(n,value);
}

template<class T,class Alloc>
template<class InputIterator>
void vector<T,Alloc>::vector_construct(InputIterator first,
    InputIterator last,__false_type){
    allocate_and_copy(first,last);
}

template <class T, class Alloc>
vector<T, Alloc>::vector(const vector<T, Alloc>& x) {
	allocate_and_copy(x.begin(), x.end());
}

template <class T, class Alloc>
vector<T, Alloc>::vector(vector<T, Alloc>&& x) {
	start = x.start;
    finish = x.finish;
    end_of_storage = x.end_of_storge;
    x.start = x.finish = x.end_of_storage = 0;
}

template <class T, class Alloc>
vector<T,Alloc>& vector<T,Alloc>::operator=(const vector<T,Alloc>& x){
    if(this != &x){
        const auto len = rhs.size();
        /*要重分配空间*/
        if(len > capacity()){
            destory_and_deallocate();
            allocate_and_copy(x.begin(),x.end());
        }else if(size() >= len){/*不用重新分配空间*/
            iterator it = copy(x.begin(),x.end(),start);
            ZMJ::destory(it,finish);
        }else{/*不用重新分配空间，但需要扩充finish。*/
            copy(x.begin(),x.begin() + size(),start);
            ZMJ::uninitialized_copy(x.begin() + size(),x.end(),finish);
        }
        finish = start + len;
    }
    return *this;
}

template <class T, class Alloc>
vector<T,Alloc>& vector<T,Alloc>::operator=(vector<T,Alloc>&& x){
    if(this != &x){
        destory_and_deallocate();
        start = x.start;
        finish = x.finish;
        end_of_storage = x.end_of_storage;
        x.start = x.finish = x.end_of_storage = 0;
    }
    return *this;
}

template <class T, class Alloc>
void vector<T,Alloc>::resize(size_type new_size,const T& value){
    if(new_size < size())
        earse(begin() + new_size,end());
    else
        insert(end(),new_size - size(),value);/*从何处开始，插入多少个，插入值是什么*/
}

template <class T, class Alloc>
void vector<T,Alloc>::push_back(const T& value){
    if(finish != end_of_storage){
        construct(finish,value);
        ++finish;
    }
    else{
        insert_aux(end(), value);
    }
}

template <class T, class Alloc>
void vector<T,Alloc>::pop_back(){
    if(finish != start){
        destory(finish);
        --finish;//可能析构
    }
}

template <class T, class Alloc>
vector<T,Alloc>::iterator 
vector<T,Alloc>::earse(iterator position){
    if(position + 1 != end())
        copy(position+1,finish,position);
    /*
    *   这真的很奇怪，position位置元素不会被覆盖了吗？
    * 如果担心直接内存copy，而导致position内容被覆盖，并且没有调用它的析构。
    * 这个不用担心，因为copy里面使用has_trivial_assignment_construct,
    * 来决定是使用直接内存memmove还是对copy内容使用赋值构造。
    * 
    *   至于赋值过程中，对象的资源是释放还是移动，那是类类型作者该考虑的，
    * 而不是容器作者该考虑的。
    */
    --finish;
    destory(finish);
    return position;
}

template <class T, class Alloc>
vector<T,Alloc>::iterator 
vector<T,Alloc>::earse(iterator first,iterator last){
    iterator it = copy(last,finish,first);
    data_allocator::destory(it,finish);
    finish = finish - (last - first);
    return first;
}

template <class T, class Alloc>
void vector<T,Alloc>::swap(vector<T,Alloc>& rhs){
    ZMJ::swap(start,rhs.start);
    ZMJ::swap(finish,rhs.finish);
    ZMJ::swap(end_of_storage,rhs.end_of_storage);
}

template <class T, class Alloc>
void vector<T,Alloc>::swap(vector<T,Alloc>& x,vector<T,Alloc>& y){
    x.swap(y);
}

template <class T, class Alloc>
void vector<T,Alloc>::destory_and_deallocate(){
    destory(start,finish);
    if(start) data_allocator::deallocate(start,end_of_storage - start);
}

template <class T, class Alloc>
void vector<T,Alloc>::allocate_and_fill(size_type n,const T& value){
    start = data_allocator::allocate(n);
    finish = uninitialized_fill_n(start,n,value);
    end_of_storage = start + n;
}

/*配置空间并复制内容,返回容器开始的迭代器*/
template <class T, class Alloc>
vector<T,Alloc>::iterator vector<T,Alloc>::allocate_and_copy(iterator first,iterator last){
    difference_type n = last - first;
    start = data_allocator::allocate(n);
    finish = uninitialized_copy(first,last,start);
    end_of_storage = finish;
    return start;
}

// insert_aux 函数
template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x){
    if(size() + 1 <= capacity()){ /*有剩余空间*/
        construct(finish,back());
        ++finish;
        /*原先最后一个元素已经被构造，所以只用移动[position,finish-2)到finish-1 */
        copy_backward(position,finish - 2,finish - 1);
        T x_copy = x;
        *position = x_copy;
    }else{
        const size_type old_size = size();
        const size_type new_size = old_size == 0 ? 1 : 2 * old_size;

        iterator new_start = data_allocator::allocate(new_size);
        iterator new_finish = new_start;

        /*
        *   这里前面分配内存不try catch，进行初始化反而进行try catch。
        * 这里是遵守要么全部初始化完成，要么一个都没完成。应该是预防初始化，
        * 出现异常，然后把前面初始化的析构，分配的内存释放。
        * 
        * 至于分配内存为什么不放假try块内，因为就是异常了无需任何处理啊，
        * 直接让它传递往上就可以了。
        */
        try{
            new_finish = uninitialized_copy(start,position,new_start);
            construct(new_finish,x);
            ++new_finish;
            new_finish = uninitialized_copy(position,finish,new_finish);
        }catch(...){
            destroy(new_start,new_finish);
            data_allocator::deallocate(new_start,new_size);
            throw;
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = start + new_size;
    }
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n,const T& x){
    if(n == 0) return ;
    if(size_type(end_of_storage - finish) >= n){
        T x_copy = x;
        const size_type after_elems = finish - position;
        iterator old_finish = finish;
        if(after_elems > n){
            uninitialized_copy(finish - n,finish,finish);
            finish += n;
            copy_backward(position,old_finish - n,old_finish);
            uninitialized_fill_n(position,n,x_copy);
        }else{
            uninitialized_fill_n(finish,n - after_elems,x_copy);
            finish += n - after_elems;
            uninitialized_copy(position,old_finish,finish);
            finish += after_elems;
            uninitialized_fill_n(position,after_elems,x_copy);
        }
    }else{
        size_type old_size = size();
        size_type new_size = old_size + ZMJ::max(old_size,n);

        iterator new_start = data_allocator::allocate(new_size);
        iterator new_finish = new_start;

        try{
            new_finish = uninitialized_copy(start,start + (position - start),new_finish);
            new_finish = uninitialized_fill_n(new_finish,n,x);
            new_finish = uninitialized_copy(position,finish,new_finish);
        }catch(...){
            destory(new_start,new_finish);
            data_allocator::deallocate(new_finish,new_size);
            throw;
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = start + new_size;
    }
}


template<class T,class Alloc>
inline bool operator==(vector<T,Alloc>& x,vector<T,Alloc>& y){
    return x.size() == y.size() && equal(x.begin(),x.end(),y.begin());
}

template<class T,class Alloc>
inline bool operator<(vector<T,Alloc>& x,vector<T,Alloc>& y){
    return lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
}

template<class T,class Alloc>
inline bool operator==(vector<T,Alloc>& x,vector<T,Alloc>& y){
    return !(x == y);
}

template<class T,class Alloc>
inline bool operator>(vector<T,Alloc>& x,vector<T,Alloc>& y){
    return y < x;
}

template<class T,class Alloc>
inline bool operator>=(vector<T,Alloc>& x,vector<T,Alloc>& y){
    return !(x < y);
}

template<class T,class Alloc>
inline bool operator<=(vector<T,Alloc>& x,vector<T,Alloc>& y){
    return !(y < x);
}

}// namespace ZMJ
#endif // ！__VECTOR_H__