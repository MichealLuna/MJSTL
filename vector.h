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
    using value_type            = T;
    using pointer               = T*;
    using reference             = T&;
    using const_pointer         = const T*;
    using const_reference       = const T&;
    using iterator              = T*;
    using const_iterator        = const T*;
    using size_type             = size_t;
    using difference_type       = ptrdiff_t;

protected:
    using data_allocator        = Alloc;
    using reverse_iterator      = reverse_iterator<iterator>;

    using self                  = vector<T,Alloc>;
protected:
    iterator start;
    iterator finish;
    iterator end_of_storage;

    /*插入元素*/
    void insert_aux(iterator position,const T& x);

    /*初始化：插入n个值为value的元素。*/
    void deallocate(){
        if(start)
            data_allocator::deallocate(start,end_of_storage-start);
    }

    /*初始化：插入n个值为value的元素。*/
    void fill_initialize(size_type n,const T& value){
        start = allocate_and_fill(n,value);
        finish = start + n;
        end_of_storage = finish;
    }

    /*配置空间并填充内容*/
    iterator allocate_and_fill(size_type n,const T& value){
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result,n,value);
        return result;
    }

    /*配置空间并复制内容*/
    iterator allocate_and_copy(size_type n,const_iterator first,const_iterator last){
        iterator result = data_allocator::allocate(n);
        try{
            /*将原来[first,last)复制到last中去。*/
            uninitialized_copy(first,last,result);
            return result;
        }
    }

public:
    vector():start(nullptr),finish(nullptr),end_of_storage(nullptr){}

    vector(size_t n, const T& value){
        fill_initialize(n,value);
    }

    vector(long n,const T& value){
        fill_initialize(n,value);
    }

    explicit vector(size_type n){
        fill_initialize(n,T());
    }

    /*copy construct*/
    vector(const vector<T,Alloc>& rhs){
        fill_initialize(x.size(),T());
        finish = uninitialized_copy(x.begin(),x.end(),start);
    }

    /*assignment copy*/
    vector<T,Alloc>& operator=(const vector<T,Alloc>& rhs);

    vector(vector<T,Alloc>&& rhs){
        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;
        rhs.start = nullptr;
        rhs.finish = nullptr;
        rhs.end_of_storage = nullptr;
    }

    ~vector(){
        destory(start,finish);
        deallocate();
    }

public:
    iterator begin() const{
        return start;
    }

    iterator end() const{
        return finish;
    }

    /*想了想还是使用函数调用获得容器好，利于更改。*/
    reverse_iterator rbegin() const{
        return reverse_iterator(end());
    }

    reverse_iterator rend() const{
        return reverse_iterator(begin());
    }

    size_type size() const{
        return size_type(end() - begin());
    }

    size_type max_size() const{
        return size_type(-1)/sizeof(T);
    }

    size_type capacity() const{
        return size_type(end_of_storage - begin());
    }

    bool empty() const{
        return begin() == end();
    }

    void reserve(size_type n){

    }
    void resize(size_type n);
    void resize(size_type n, const T& value);

    reference operator[](size_type n){
        return *(begin() + n);
    }
    const_reference operator[](size_type n) const{
        return (const_reference)start[n];
    }

    reference at(size_type n){
        assert(start <= start + n && start + n < end_of_storage);
        return (*this)[n];
    }
    const_reference at(size_type n) const{
        assert(start <= start + n && start + n < end_of_storage);
        return (*this)[n];
    }

    reference front(){
        return *begin();
    }

    const_reference front() const{
        return (const_reference)(*begin());
    }

    reference back(){
        return *(end() - 1);
    }

    const_reference back() const{
        return (const_reference)(*(end() - 1));
    }

    iterator data() const{
        return start;
    }

    void push_back(const T& value){
        if(finish != end_of_storage){
            construct(finish,value);
            ++finish;
        }
        else{
            insert_aux(end(), value);
        }
    }

    void push_back(T&& value){
        if(finish != end_of_storage){
            *finish = value;
            ++finish;//前提是T类型有自己的移动构造，否则无用。
        }else{
            insert_aux(end(),value);/*应该转发。*/
        }
    }

    void pop_back(){
        if(finish != start){
            destory(finish);
            --finish;//可能析构
        }
    }

    iterator earse(iterator position){
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

    iterator earse(iterator first,iterator last){
        iterator it = copy(last,finish,first);
        data_allocator::destory(it,finish);
        finish = finish - (last - first);
        return first;
    }

    void clear(){
        /*如果T 是has_trivial_destruct，直接finish = start;否则应该对每个元素调用destroy*/
    }

    void resize(size_type new_size,const T& value){
        if(new_size < size())
            earse(begin() + new_size,end());
        else
            insert(end(),new_size - size(),value);/*从何处开始，插入多少个，插入值是什么*/
    }

    void resize(size_type new_size){
        resize(new_size,T());
    }

    void insert(iterator pos, size_type n,const T& value);

    inline void swap(vector<T,Alloc>& rhs){
        ZMJ::swap(start,rhs.start);
        ZMJ::swap(finish,rhs.finish);
        ZMJ::swap(end_of_storage,rhs.end_of_storage);
    }

    inline void swap(vector<T,Alloc>& x,vector<T,Alloc>& y){
        x.swap(y);
    }

    inline bool operator==(vector<T,Alloc>& x,vector<T,Alloc>& y){
        return x.size() == y.size() && equal(x.begin(),x.end(),y.begin());
    }

    inline bool operator<(vector<T,Alloc>& x,vector<T,Alloc>& y){
        return lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
    }

    template<class T,class Alloc = simple_alloc<T>>
    inline bool operator==(vector<T,Alloc>& x,vector<T,Alloc>& y){
        return !(x == y);
    }

    inline bool operator>(vector<T,Alloc>& x,vector<T,Alloc>& y){
        return y < x;
    }

    inline bool operator>=(vector<T,Alloc>& x,vector<T,Alloc>& y){
        return !(x < y);
    }

    inline bool operator<=(vector<T,Alloc>& x,vector<T,Alloc>& y){
        return !(y < x);
    }

    vector<T,Alloc>& operator=(const vector<T>& rhs){
        if(this == &rhs) return *this;
        const auto len = rhs.size();

        /*要重分配空间*/
        if(len > capacity()){
            iterator it = allocate_and_copy(len,rhs.begin(),rhs.end());
            ZMJ::destory(start,finish);
            this->deallocate();
            start = it;
            end_of_storage = start + len;
        }else if(size() >= len){/*不用重新分配空间*/
            iterator it = copy(rhs.begin(),rhs.end(),start);
            ZMJ::destory(it,finish);
        }else{/*不用重新分配空间，但需要扩充finish。*/
            copy(rhs.begin(),rhs.begin() + size(),start);
            ZMJ::uninitialized_copy(rhs.begin() + size(),rhs.end(),finish);
        }
        finish = start + len;
    }
};


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

        destory(start,finish);
        this->deallocate();

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

        destory(start,finish);
        this->deallocate();

        start = new_start;
        finish = new_finish;
        end_of_storage = start + new_size;
    }
}

}//namespace ZMJ


#endif //vector.h