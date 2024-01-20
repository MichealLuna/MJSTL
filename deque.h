#ifndef __DEQUE_H__
#define __DEQUE_H__

#include <cassert>
#include <initializer_list>

#include "iterator.h"
#include "reverse_iterator.h"
#include "memory.h"

namespace mjstl
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
        typedef __deque_iterator<T,T&,T*,BufSize>               iterator;
        typedef __deque_iterator<T,const T&,const T*,BufSize>   const_iterator;
        static size_t buffer_size() { return __deque_buf_size(BufSize,sizeof(T));}

        typedef T               value_type;
        typedef T*              pointer;
        typedef T&              reference;
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;
        typedef T**             map_pointer;

        typedef __deque_iterator self;

        T* cur;     /*指向缓冲区的当前元素。*/
        T* first;   /*指向缓冲区的头部。*/
        T* last;    /*指向缓冲区的尾部。*/
        map_pointer node;

        /*constructor*/
        __deque_iterator():cur(0),first(0),last(0),node(0){}
        __deque_iterator(T* x,map_pointer y):cur(x),first(*y),
            last(*y + buffer_size()),node(y){};
        __deque_iterator(const iterator& x):cur(x.cur),
            first(x.first),last(x.last),node(x.node){}

        __deque_iterator(iterator&& x):
            cur(x.cur),first(x.first),last(x.last),node(x.node){
            x.cur = nullptr;
            x.first = nullptr;
            x.last = nullptr;
            x.node = nullptr;
        }

        self& operator=(const iterator& x){
            if(this != &x){
                cur = x.cur;
                first = x.first;
                last = x.last;
                node = x.node;
            }
            return *this;
        }

        void set_node(map_pointer new_node){
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }

        reference operator*() const { return *cur;}
        pointer operator->() const { return &(operator*());}

        /*
        *   表示  计算*this - x的距离。
        * 为什么距离要 -(x.cur - first)；
        * 因为node - x.node把 整个x.node大小buffer_size()计入距离，
        * 所以要减去多计算的(x.cur - x.first)部分，剩下[x.cur,x.last)部分。
        */
        difference_type operator-(const self& x) const{
            return static_cast<difference_type>(difference_type(buffer_size()) * (node - x.node) + 
                (cur - first) - (x.cur - x.first));
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
            if(first == cur){
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
                    (-difference_type((-offset - 1) / buffer_size()) - 1);
                set_node(node + new_node);
                cur = first + (offset - difference_type(buffer_size() * new_node));
            }
            return *this;
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


    template<class T,class Alloc = alloc,size_t BufSize = 0>
    class deque{
    public:
        typedef mjstl::allocator<T>     data_allocator;
        typedef mjstl::allocator<T*>    map_allocator;
        typedef mjstl::allocator<T>     allocate_type;
    public:
        /*deque嵌套型别定义。*/
        typedef T                   value_type;
        typedef value_type*         pointer;
        typedef const value_type*   const_pointer;
        typedef value_type&         reference;
        typedef const value_type&   const_reference;
        /*重点表达容器内元素的个数。*/
        typedef size_t              size_type;
        /*重点表达容器的相对距离，一般跟编译的系统位数有关，而且可能是有符号的！*/
        typedef ptrdiff_t           difference_type;

        typedef __deque_iterator<T,T&,T*,BufSize> iterator;
        typedef __deque_iterator<T,const T&,const T*,BufSize> const_iterator;
        /*
        *   这里必须使用mjstl前缀，因为这里刚定义了一个reverse_iterator，
        * 编译器优先找类范围的reverse_iterator.
        * */
        typedef mjstl::reverse_iterator<iterator>  reverse_iterator;
        typedef mjstl::reverse_iterator<const_iterator> const_reverse_iterator;

        static size_t buffer_size(){ return __deque_buf_size(BufSize,sizeof(T));}

    protected:
        typedef pointer* map_pointer;
        iterator start; /*指向第一个节点。*/
        iterator finish;/*指向最后一个节点。*/
        map_pointer map;/*指向一块map区域，map内都是指针，指向一个缓冲区。*/
        size_type map_size;/*map内的指针数量*/
        enum{ __initial_map_size = 8};

    public:
        /*constructor*/
        deque() { __fill_initialize(0,T());}
        deque(size_type n,const T& value){ __fill_initialize(n,value);}
        explicit deque(size_type n){ __fill_initialize(n,T());}
        template<class InputIterator,typename std::enable_if<
            mjstl::is_input_iterator<InputIterator>::value,int>::type = 0>
        deque(InputIterator first,InputIterator last);
        deque(std::initializer_list<value_type> ilist);

        /*copy constructor*/
        deque(const deque& x);
        deque(deque&& x);

        /*assignment operation*/
        deque& operator=(const deque& x);
        deque& operator=(deque&& x);
        deque& operator=(std::initializer_list<value_type> ilist);
        /*destructor*/
        ~deque();

    public:
        /*about iterator*/
        iterator begin(){ return start;}
        const_iterator begin()const { return start;}
        iterator end() {return finish;}
        const_iterator end() const { return finish;}
        reverse_iterator rbegin() { return reverse_iterator(end());}
        const_reverse_iterator rbegin() const { return reverse_iterator(end());}
        reverse_iterator rend() { return reverse_iterator(begin());}
        const_reverse_iterator rend() const { return reverse_iterator(begin());}

        /*about container operation*/
        bool empty() const noexcept { return begin() == end();}
        size_type size() const { return finish - start;}
        size_type max_size() const { return size_type(-1)/sizeof(T);}

        /*about access element operation*/
        reference front() { return *begin();}
        const_reference front() const { return *begin();}
        reference back() { return *(end() - 1);}
        const_reference back() const { return *(end() - 1);}
        reference operator[](size_type n){ return start[difference_type(n)];}
        const_reference operator[](size_type n) const { return start[difference_type(n)];}
        reference at(size_type n){ assert(n>=0 && n < size()); return (*this)[n];}
        const_reference at(size_type n) const { assert(n>=0 && n < size()); return (*this)[n];}

        /*assign*/
        void assign(size_type n,const T& value){ __fill_assign(n,value);}
        void assign(size_type n){ __fill_assign(n,T());}
        void assign(std::initializer_list<value_type> ilist);
        template<class InputIterator,typename std::enable_if<
            mjstl::is_input_iterator<InputIterator>::value,int>::type = 0>
        void assign(InputIterator first,InputIterator last);
        iterator insert(iterator position,const T& x);
        iterator insert(iterator position){ insert(position,T());}
        void insert(iterator position,size_type n,const T& x);
        template<class InputIterator>
        void insert(iterator position,InputIterator first,InputIterator last);
        iterator erase(iterator position);
        iterator erase(iterator first,iterator last);
        void clear();
        void push_back(const T& x);
        void push_front(const T& x);
        void pop_back();
        void pop_front();
        void resize(size_type new_size,const T& x);
        void resize(size_type new_size){ resize(new_size,T());}
        void swap(deque& x);

        allocate_type get_allocate() { return allocate_type();}
    
    protected:
        void __create_node(map_pointer nstart,map_pointer nfinish);
        void __destory_node(map_pointer nstart,map_pointer nfinish);
        void __map_initialize(size_t nelem);
        void __fill_initialize(size_type n,const T& value);
        template<class InputIterator>
        void __range_initialize(InputIterator,InputIterator,input_iterator_tag);
        template<class ForwardIterator>
        void __range_initialize(ForwardIterator,ForwardIterator,forward_iterator_tag);
        template<class Integer>
        void __initialize_dispatch(Integer n,Integer value,__true_type);
        template<class InputIterator>
        void __initialize_dispatch(InputIterator first,InputIterator last,__false_type);
        void __fill_assign(size_type n,const T& value);
        template<class Integer>
        void __assign_dispatch(Integer n,Integer value,__true_type);
        template<class InputIterator>
        void __assign_dispatch(InputIterator first,InputIterator last,__false_type);
        template<class InputIterator>
        void __assign_aux(InputIterator first,InputIterator last,input_iterator_tag);
        template<class ForwardIterator>
        void __assign_aux(ForwardIterator first,ForwardIterator last,forward_iterator_tag);
        iterator __insert_aux(iterator position,const T& x);
        void __fill_insert(iterator position,size_type n,const T& x);
        void __fill_insert_aux(iterator position,size_type n,const T& x);
        template<class ForwardIterator>
        void __fill_insert_aux(iterator,ForwardIterator,ForwardIterator,size_type);
        template<class Integer>
        void __insert_dispatch(iterator,Integer,Integer,__true_type);
        template<class InputIterator>
        void __insert_dispatch(iterator position,InputIterator first,InputIterator last,__false_type);
        template<class InputIterator>
        void __insert_dispatch_aux(iterator,InputIterator,InputIterator,input_iterator_tag);
        template<class ForwardIterator>
        void __insert_dispatch_aux(iterator,ForwardIterator,ForwardIterator,forward_iterator_tag);
        void __push_back_aux(const T& x);
        void __push_front_aux(const T& x);
        void __pop_back_aux();
        void __pop_front_aux();
        iterator __reserve_elements_at_back(size_type n);
        iterator __reserve_elements_at_front(size_type n);
        void __reserve_map_at_back(size_type n = 1);
        void __reserve_map_at_front(size_type n = 1);
        void __reallocate_map(size_type node_to_add,bool add_at_front);
    };


/*constructor*/
template<class T,class Alloc,size_t BufSize>
template<class InputIterator,typename std::enable_if<
    mjstl::is_input_iterator<InputIterator>::value,int>::type>
deque<T,Alloc,BufSize>::deque(InputIterator first,InputIterator last){
    typedef typename __is_integer<InputIterator>::is_integer integer;
    __initialize_dispatch(first,last,integer());
}

/*copy constructor*/
template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>::deque(const deque<T,Alloc,BufSize>& x){
    __map_initialize(x.size());
    mjstl::uninitialized_copy(x.begin(),x.end(),start);
}

template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>::deque(std::initializer_list<value_type> ilist){
    __map_initialize(size_type(ilist.size()));
    mjstl::uninitialized_copy(ilist.begin(),ilist.end(),start);
}

template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>::deque(deque<T,Alloc,BufSize>&& x):
    start(std::move(x.start)),
    finish(std::move(x.finish)),
    map(x.map),
    map_size(x.map_size)
{
    x.finish = x.start;
    x.map = nullptr;
    x.map_size = 0;
}

/*assignment operator*/
template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>& deque<T,Alloc,BufSize>::operator=(const deque<T,Alloc,BufSize>& x){
    size_type len = size();
    if(this != &x){
        if(len >= x.size()){
            erase(mjstl::copy(x.begin(),x.end(),start),finish);
        }else{
            const_iterator mid = x.begin() + difference_type(len);
            mjstl::copy(x.begin(),mid,start);
            insert(finish,mid,x.end());
        }
    }
    return *this;
}

template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>& 
deque<T,Alloc,BufSize>::operator=(deque<T,Alloc,BufSize>&& x){
    clear();
    // data_allocator::deallocate(*(start.node),buffer_size());
    // map_allocator::deallocate(map,map_size);

    start = std::move(x.start);
    finish = std::move(x.finish);
    map = x.map;
    map_size = x.map_size;

    x.finish = x.start;
    x.map = nullptr;
    x.map_size = 0;
    return *this;
}

template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>& 
deque<T,Alloc,BufSize>::operator=(std::initializer_list<value_type> ilist){
    deque<T,Alloc,BufSize> tmp(ilist);
    swap(tmp);
    return *this;
}

/*destructor*/
template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>::~deque(){
    if(map){
        /*这里finish的node指向最后区块，而区块数组是最后一块的下一块，左闭右开原则。*/
        __destory_node(start.node,finish.node + 1);
        map_allocator::deallocate(map,map_size);
    }
}

/*initialize container*/
template<class T,class Alloc,size_t BufSize>
template<class InputIterator,typename std::enable_if<
        mjstl::is_input_iterator<InputIterator>::value,int>::type>
void deque<T,Alloc,BufSize>::assign(InputIterator first,InputIterator last){
    typedef typename __is_integer<InputIterator>::is_integer integer;
    __assign_dispatch(first,last,integer());
}

/**
 *     这里使用了自带的初始化列表，得明确__is_integer是判断
 * 初始化列表的迭代器std::initializer_list<value_type>::iterator
 * 如果__is_integer<value_type>::is_integer它将一直为真。
*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::assign(std::initializer_list<value_type> ilist){
    typedef typename std::initializer_list<value_type>::iterator it;
    typedef typename __is_integer<it>::is_integer integer;
    __assign_dispatch(ilist.begin(),ilist.end(),integer());  
}
/*insert element at position*/
template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::insert(iterator position,const T& x){
    if(position.cur == start.cur){
        push_front(x);
        return start;
    }else if(position.cur == finish.cur){
        push_back(x);
        /*这里的finish是末尾标志位，需要-1才是刚插入的元素位置。*/
        iterator tmp = finish;
        --tmp;
        return tmp;
    }else
        return __insert_aux(position,x);
}

/*insert nth elements at position*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::insert(iterator position,size_type n,const T& x){
    __fill_insert(position,n,x);
}

/*insert range [first,last) elements at position*/
template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::insert(iterator position,
    InputIterator first,InputIterator last){
    typedef typename __is_integer<InputIterator>::is_integer integer;
    __insert_dispatch(position,first,last,integer());
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::erase(iterator position){
    iterator next = position;
    ++next;
    size_type index = static_cast<size_type>(position - start);
    if(index < (size() >> 1)){
        mjstl::copy_backward(start,position,next);
        pop_front();/*这个会析构原来，在释放。没错。*/
    }else{
        mjstl::copy(next,finish,position);
        pop_back();
    }
    return start + index;
}

template <class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::erase(iterator first,iterator last){
    if(first == start && finish == last){
        clear();
        return finish;
    }else{
        size_type n = static_cast<size_type>(last - first);
        size_type elem_before = static_cast<size_type>(first - start);
        if(elem_before < (size() - n) / 2){
            mjstl::copy_backward(start,first,last);
            iterator new_start = start + n;
            data_allocator::destory(start.cur,new_start.cur);
            /*释放缓冲区，是否必要？*/
            for(map_pointer cur = start.node; cur != new_start.node; ++cur)
                data_allocator::deallocate(*cur,buffer_size());
            start = new_start;
        }else{
            mjstl::copy(last,finish,first);
            iterator new_finish = finish - n;
            data_allocator::destory(new_finish.cur,finish.cur);
            /*finish也在具体缓冲块中，不能直接删除。*/
            for(map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                data_allocator::deallocate(*cur,buffer_size());
            finish = new_finish;
        }
        return start + elem_before;
    }
}

/*clear all elements in container,but ramains a block buffer to hold finish.*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::clear(){
    /*此处，先释放start和finish之间的map_pointer所指的缓冲区的元素。*/
    for(map_pointer cur = start.node + 1; cur < finish.node; ++cur){
        data_allocator::destory(*cur,*cur + buffer_size());
        data_allocator::deallocate(*cur,buffer_size());
        *cur = nullptr;
    }

    /*分情况释放start和finish缓冲区的元素。*/
    /*如果不在同一块*/
    if(start.node != finish.node){
        data_allocator::destory(start.cur,start.last);
        data_allocator::destory(finish.first,finish.cur);
        data_allocator::deallocate(*(finish.node),buffer_size());
        *(finish.node) = nullptr;
    }else{
        data_allocator::destory(start.cur,finish.cur);
    }
    finish = start;
}

/*insert an element at the end of container*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::push_back(const T& x){
    if(finish.cur != finish.last - 1){
        data_allocator::construct(finish.cur,x);
        ++finish.cur;
    }else
        __push_back_aux(x);
}

/*insert an element at the begin of container*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::push_front(const T& x){
    if(start.cur != start.first){
        /*这里应该先自减1再进行构造？*/
        data_allocator::construct(start.cur - 1,x);
        --start.cur;
    }else
        __push_front_aux(x);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::pop_back(){
    if(finish.cur != finish.first){
        --finish.cur;
        data_allocator::destory(finish.cur);
    }else
        __pop_back_aux();
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::pop_front(){
    /*左闭右开，这里start.cur是不可能为start.last的，最多start.last-1。*/
    if(start.cur != start.last - 1){
        data_allocator::destory(start.cur);
        ++start.cur;
    }else
        __pop_front_aux();
}

/*resize*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::resize(size_type new_size,const T& x){
    const size_type len = size();
    if(new_size < len)
        erase(start + new_size,finish);
    else
        insert(finish,new_size - len,x);
}

/*swap*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::swap(deque& x){
    if(this == &x) return;
    mjstl::swap(start,x.start);
    mjstl::swap(finish,x.finish);
    mjstl::swap(map,x.map);
    mjstl::swap(map_size,x.map_size);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__create_node(map_pointer nstart,map_pointer nfinish){
    map_pointer cur;
    try{
        for(cur = nstart; cur <= nfinish; ++cur)
            *cur = data_allocator::allocate(buffer_size());
    }catch(...){
        __destory_node(nstart,cur);
        throw;
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__destory_node(map_pointer nstart,map_pointer nfinish){
    /*这里释放内存还保留了最后一个缓冲块*/
    for(map_pointer n = nstart; n < nfinish; ++n){
        data_allocator::deallocate(*n,buffer_size());
        *n = nullptr;
    }
}  

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__map_initialize(size_t nElem){
    /*至少分配1个用来做finish。*/
    size_type nNode = nElem / buffer_size() + 1;
    /*为什么+2？*/
    map_size = mjstl::max((size_type)__initial_map_size,nNode + 2);
    map = map_allocator::allocate(map_size);
    /*让nstart,nfinish都指向map最中央的区域，方便向两边扩充。*/
    /* map_size - nNode 意思是把nstart,nfinish放到居中的位置。*/
    map_pointer nstart = map + (map_size - nNode)/2;
    map_pointer nfinish = nstart + nNode - 1;
    /*
    *   当nNode >= 6 时，map_size >= 8; 
    *       (map_size - nNode) = 2; nNode - 1为最后一个。
    *   当nNode < 6时，map_size = 8;
    *       2 < (map_size - nNode) <= 7; 
    * */
    try{
        __create_node(nstart,nfinish);
    }catch(...){
        map_allocator::deallocate(map,map_size);
        map = 0;
        map_size = 0;
    }
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    finish.cur = finish.first + nElem % buffer_size();
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__fill_initialize(size_type n,const T& value){
    __map_initialize(n);
    map_pointer cur;
    try{
        for(cur = start.node; cur < finish.node; ++cur)
            mjstl::uninitialized_fill(*cur,*cur + buffer_size(),value);
        mjstl::uninitialized_fill(finish.first,finish.cur,value);
    }catch(...){
        mjstl::destory(start,iterator(*cur,cur));
        throw;
    }
}

template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__range_initialize(InputIterator first,
    InputIterator last,input_iterator_tag){
    __map_initialize(0);
    try{
        /*这里因为输入是单向迭代器，只能一个一个push_back。*/
        for(; first != last; ++first)
            push_back(*first);
    }catch(...){
        clear();
        throw;
    }
}

template<class T,class Alloc,size_t BufSize>
template<class ForwardIterator>
void deque<T,Alloc,BufSize>::__range_initialize(ForwardIterator first,
    ForwardIterator last,forward_iterator_tag){
    size_type n = distance(first,last);
    __map_initialize(n);
    map_pointer cur;
    try{
        for(cur = start.node; cur < finish.node; ++cur){
            ForwardIterator mid = first;
            advance(mid,buffer_size());//前进一个块内的元素。
            mjstl::uninitialized_copy(first,mid,*cur);
            first = mid;//前进。
        }
        mjstl::uninitialized_copy(first,last,finish.first);
    }catch(...){
        mjstl::destory(start,iterator(*cur,cur));
    }
}

template<class T,class Alloc,size_t BufSize>
template<class Integer>
void deque<T,Alloc,BufSize>::__initialize_dispatch(Integer n,Integer value,__true_type){
    __fill_initialize(n,value);
}

template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__initialize_dispatch(InputIterator first,
    InputIterator last,__false_type){
    __range_initialize(first,last,iterator_category(first));
}

/*这是会在容器填充n个元素，先把已经存在的元素变成value，如果还不够n个元素再插入足够的value。*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__fill_assign(size_type n,const T& value){
    if(n > size()){
        mjstl::fill(begin(),end(),value);
        insert(end(),n - size(),value);
    }else{
        erase(begin() + n,end());
        mjstl::fill(begin(),end(),value);
    }
}

/*不合适*/
template<class T,class Alloc,size_t BufSize>
template<class Integer>
void deque<T,Alloc,BufSize>::__assign_dispatch(Integer n,Integer value,__true_type){
    __fill_assign(n,value);
}

/*
*   这里用到的__assign_aux跟前面的__fill_assign有什么区别？
* __fill_assign用于整数，而__assign_aux可以用于对象类型，
* 并且__fill_assign和__assign_aux的参数意义不一样，前者表示n个
* 为value的整数，后者表示[first,last)区间的元素。
*
*   assign可能涉及到删除元素，
*/
template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__assign_dispatch(InputIterator first,
    InputIterator last,__false_type){
    __assign_aux(first,last,iterator_category(first));
}

template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__assign_aux(InputIterator first,
    InputIterator last,input_iterator_tag){
    iterator first1 = begin();
    iterator last1 = end();
    for(; first1 != last1 && first != last; ++first1,++first)
        *first = *first1;
    
    if(first != last)
        erase(first,last);
    else
        insert(end(),first,last);
}

template<class T,class Alloc,size_t BufSize>
template<class ForwardIterator>
void deque<T,Alloc,BufSize>::__assign_aux(ForwardIterator first,
    ForwardIterator last,forward_iterator_tag){
    size_type len = distance(first,last);
    if(len > size()){
        ForwardIterator mid = first;
        advance(mid,size());
        mjstl::copy(first,mid,start);
        insert(finish,mid,last);
    }else
        erase(mjstl::copy(first,last,start),finish);
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::__insert_aux(iterator position,const T& x){
    size_type index = static_cast<size_type>(mjstl::abs(position - start));
    value_type x_copy = x;
    /*前半段短，移动前半段。*/
    if(index < (size() / 2)){
        push_front(front());
        iterator origin_first = start;
        ++origin_first;
        iterator move_first = origin_first;
        ++move_first;
        iterator move_last = position;
        ++move_last;
        mjstl::copy(move_first,move_last,origin_first);
    }else{
        /*后半段短，移动后半段。*/
        push_back(back());
        iterator origin_last = finish;
        --origin_last;
        iterator move_last = origin_last;
        --move_last;
        iterator move_first = position;
        mjstl::copy_backward(move_first,move_last,origin_last);
    }
    *position = x_copy;
    return position;
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__fill_insert(iterator position,size_type n,const T& x){
    if(start.cur == position.cur){
        iterator new_start = __reserve_elements_at_front(n);
        try{
            mjstl::uninitialized_fill(new_start,start,x);
            start = new_start;
        }catch(...){
            __destory_node(new_start.node,start.node);
        }
    }else if(finish.cur == position.cur){
        iterator new_finish = __reserve_elements_at_back(n);
        try{
            mjstl::uninitialized_fill(finish,new_finish,x);
            finish = new_finish;
        }catch(...){
            /*不能释放掉原来的finish，第二个参数遵循左闭右开原则，*/
             __destory_node(finish.node + 1,new_finish.node + 1);
        }
    }else
        __fill_insert_aux(position,n,x);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__fill_insert_aux(iterator position,size_type n,const T& x){
    size_type elements_before = static_cast<size_type>(mjstl::abs(position - start));
    size_type len = size();
    value_type x_copy = x;
    if(elements_before < size_type(len/2)){
        iterator old_start = start;
        iterator new_start = __reserve_elements_at_front(n);
        position = start + elements_before;

        try{
            if(elements_before >= size_type(n)){
                iterator start_n = start + difference_type(n);
                /*
                *   插入时向前后扩展，迭代器不会失效，但是迭代器指向的元素在容器位置变了。
                */
                mjstl::uninitialized_copy(start,start_n,new_start);
                start = new_start;
                /*
                *   这里需要copy，是因为需要覆盖原来的对象，使用赋值操作符,
                * 这样不必对要覆盖的对象调用析构，然后又调用构造。
                */
                mjstl::copy(start_n,position,old_start);
                /*
                *   这里对空出来的空间，进行填充插入值，这里fill会调用赋值构造。
                */
                mjstl::fill(position - difference_type(n),position,x_copy);
            }else{
                /*在start之前的空间都是已分配好的空间，可以使用uninitialized_fill。*/
                mjstl::uninitialized_fill(mjstl::uninitialized_copy(start,position,new_start),
                    start,x_copy);
                start = new_start;
                /*
                *   在[start,position)这一段是已经有对象了，不能直接调用uninitialized系列函数。
                * 否则会出现，对象被覆盖，又没有调用析构函数，出现未定义行为。
                */
                mjstl::fill(old_start,position,x_copy);
            }
        }catch(...){
            __destory_node(new_start.node,start.node);
        }
    }else{
        iterator old_finish = finish;
        iterator new_finish = __reserve_elements_at_back(n);
        const size_type elements_after = len - elements_before;
        position = finish - elements_after;

        try{
            /*插入点后的元素大于插入元素个数n。*/
            if(elements_after > n){
                iterator finish_n = finish - size_type(n);
                mjstl::uninitialized_copy(finish_n,finish,finish);
                finish = new_finish;
                mjstl::copy_backward(position,finish_n,old_finish);
                mjstl::fill(position,position + size_type(n),x_copy);
            }else{
                /*插入点后的元素少于n。
                *   1、[finish,position + n)可以直接uninitialized_fill;
                *   2、使用uninitialized_copy移动[position,old_finish) 
                *       到 [position + n,new_finish)；
                *   3、在使用fill填充步骤2移走的区域[position,old_finish)
                */
                iterator finish_n = position + size_type(n);
                mjstl::uninitialized_fill(finish,finish_n,x_copy);
                mjstl::uninitialized_copy(position,finish,finish_n);
                finish = new_finish;
                mjstl::fill(position,old_finish,x_copy);
            }
        }catch(...){
            __destory_node(old_finish.node + 1,new_finish.node + 1);
        }
    }
}

template<class T,class Alloc,size_t BufSize>
template<class ForwardIterator>
void deque<T,Alloc,BufSize>::__fill_insert_aux(iterator position,
    ForwardIterator first,ForwardIterator last,size_type n){
    size_type elements_before = static_cast<size_type>(mjstl::abs(position - start));
    size_type len = size();
    if(elements_before < size_type(len/2)){
        iterator old_start = start;
        iterator new_start = __reserve_elements_at_front(n);
        position = start + elements_before;
        try{
            if(elements_before >= n){
                iterator start_n = position + n;
                mjstl::uninitialized_copy(start,start_n,new_start);
                start = new_start;
                mjstl::copy(start_n,position,old_start);
                mjstl::copy(first,last,position - difference_type(n));
            }else{
                ForwardIterator mid = first;
                advance(mid,n - elements_before);
                mjstl::uninitialized_copy(first,mid,
                    mjstl::uninitialized_copy(start,position,new_start));
                start = new_start;
                mjstl::copy(mid,last,old_start);
            }
        }catch(...){
            __destory_node(new_start.node,start.node);
        }
    }else{
        iterator old_finish = finish;
        iterator new_finish = __reserve_elements_at_back(n);
        difference_type elements_after = len - elements_before;
        position = finish - elements_after;

        try{
            if(elements_after > difference_type(n)){
                iterator finish_n = finish - difference_type(n);
                mjstl::uninitialized_copy(finish_n,finish,finish);
                finish = new_finish;
                mjstl::copy(position,finish_n,old_finish);
                mjstl::copy(first,last,position);
            }else{
                ForwardIterator mid = first;
                advance(first,difference_type(n) - elements_after);
                mjstl::uninitialized_copy(position,finish,
                    mjstl::uninitialized_copy(mid,last,finish));
                finish = new_finish;
                mjstl::copy(first,mid,position);
            }
        }catch(...){
            __destory_node(finish.node + 1,new_finish.node + 1);
        }
    }
}

template<class T,class Alloc,size_t BufSize>
template<class Integer>
void deque<T,Alloc,BufSize>::__insert_dispatch(iterator position,
    Integer n,Integer value,__true_type){
    __fill_insert(position,n,value);
}

template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__insert_dispatch(iterator position,
    InputIterator first,InputIterator last,__false_type){
    __insert_dispatch_aux(position,first,last,iterator_category(first));
}

template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__insert_dispatch_aux(iterator position,
    InputIterator first,InputIterator last,input_iterator_tag){
    mjstl::copy(first,last,iterator(*this,position));
}

template<class T,class Alloc,size_t BufSize>
template<class ForwardIterator>
void deque<T,Alloc,BufSize>::__insert_dispatch_aux(iterator position,
    ForwardIterator first,ForwardIterator last,forward_iterator_tag){
    size_type n = distance(first,last);
    if(position.cur == start.cur){
        iterator new_start = __reserve_elements_at_front(n);
        try{
            mjstl::uninitialized_copy(first,last,new_start);
            start = new_start;
        }catch(...){
            __destory_node(new_start.node,start.node);
        }
    }else if(position.cur == finish.cur){
        iterator new_finish = __reserve_elements_at_back(n);
        try{
            mjstl::uninitialized_copy(first,last,finish);
            finish = new_finish;
        }catch(...){
            __destory_node(finish.node + 1,new_finish.node + 1);
        }
    }else{
        __fill_insert_aux(position,first,last,n);
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__push_back_aux(const T& x){
    value_type x_copy = x;
    __reserve_map_at_back();
    *(finish.node + 1) = data_allocator::allocate(buffer_size());
    try{
        data_allocator::construct(finish.cur,x_copy);
        finish.set_node(finish.node + 1);
        finish.cur = finish.first;
    }catch(...){
        data_allocator::deallocate(*(finish.node + 1));
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__push_front_aux(const T& x){
    value_type x_copy = x;
    __reserve_map_at_front();
    *(start.node - 1) = data_allocator::allocate(buffer_size());
    try{
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        data_allocator::construct(start.cur,x_copy);
    }catch(...){
        ++start;
        data_allocator::deallocate(*(start.node - 1));
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__pop_back_aux(){
    data_allocator::deallocate(finish.first);
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    data_allocator::destory(finish.cur);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__pop_front_aux(){
    data_allocator::destory(start.cur);
    data_allocator::deallocate(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::__reserve_elements_at_back(size_type n){
    /*finish.last - finish.cur - 1为什么要多减1？？*/
    if(n > (size_type)(finish.last - finish.cur - 1)){
        /*使其至少为1*/
        size_type new_node = (n + buffer_size() - 1) / buffer_size();
        __reserve_map_at_back(new_node);

        size_type i;
        try{
            for(i = 1; i <= new_node; ++i)
                *(finish.node + i) = data_allocator::allocate(buffer_size());
        }catch(...){
            for(size_type j = 1; j < i; ++j)
                data_allocator::deallocate(*(finish.node + j));
        }
    }
    return finish + difference_type(n);
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::__reserve_elements_at_front(size_type n){
    /*
    *   原本写的start.cur - start.last 我认为不对啊。
    */
    size_type left = static_cast<size_type>(start.cur - start.first);
    if(n > left){
        /*使其至少为1*/
        size_type new_node = (n - left +  buffer_size() - 1) / buffer_size();
        __reserve_map_at_front(new_node);

        size_type i;
        try{
            for(i = 1; i <= new_node; ++i)
                *(start.node - i) = data_allocator::allocate(buffer_size());
        }catch(...){
            for(size_type j = 1; j < i; ++j)
                data_allocator::deallocate(*(start.node - j));
            throw;
        }
    }
    return start - static_cast<difference_type>(n);
}

/*如果不够就节点就分配。*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__reserve_map_at_back(size_type n){
    if(n + 1 > map_size - (finish.node - map))
        __reallocate_map(n,false);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__reserve_map_at_front(size_type n){
    if(n + 1 > static_cast<size_type>(start.node - map))
        __reallocate_map(n,true);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__reallocate_map(size_type node_to_add,bool add_at_front){
    /*finish.node 可能等于start.node,但是deque的map_pointer数组至少有一个缓冲区，所以+1。*/
    size_type old_nodes_num = finish.node - start.node + 1;
    size_type new_nodes_num = old_nodes_num + node_to_add;
    map_pointer new_start;
    /*map_size > 2 * new_nodes_num 说明空间足够，只是有数据的节点不够“居中”。*/
    if(map_size > 2 * new_nodes_num){
        new_start = map + (map_size - new_nodes_num) / 2
            + (add_at_front?node_to_add:0);

        /*往前移动了*/
        if(new_start < start.node)
            mjstl::copy(start.node,finish.node + 1,new_start);
        else
            mjstl::copy_backward(start.node,finish.node + 1,new_start + old_nodes_num);
    }else{
        size_type new_map_size = map_size + mjstl::max(map_size,node_to_add) + 2;
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_start = new_map + (new_map_size - new_nodes_num) / 2
            + (add_at_front?node_to_add:0);
        mjstl::copy(start.node,finish.node+1,new_start);
        map_allocator::deallocate(map,map_size);
        map = new_map;
        map_size = new_map_size;
    }

    start.set_node(new_start);
    /*这里old_nodes_num包括了原来的finish的节点，所以-1。*/
    finish.set_node(new_start + old_nodes_num - 1);
}

} // namespace mjstl
#endif// !__DEQUE_H__