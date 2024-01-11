#ifndef __DEQUE_H__
#define __DEQUE_H__

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
        typedef __queue_iterator<T,T&,T*,BufSize>               iterator;
        typedef __queue_iterator<T,const T&,const T*,BufSize>   const_iterator;
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


    template<class T,class Alloc = alloc,size_t BufSize>
    class deque{
    public:
        /*deque嵌套型别定义。*/
        typedef T   value_type;
        typedef Alloc allocate_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef __deque_iterator<T,T&,T*,BufSize> iterator;
        typedef __deque_iterator<T,const T&,const T*,BufSize> const_iterator;
        typedef reverse_iterator<iterator>  reverse_iterator;
        typedef reverse_iterator<const_iterator> const_reverse_iterator;

        static size_t buffer_size(){ return __deque_buf_size(BufSize,sizeof(T));}

    public:
        typedef allocator<T,Alloc> data_allcator;
        typedef allocator<T*,Alloc> map_allocator;

    protected:
        typedef pointer* map_pointer;
        iterator start; /*指向第一个节点。*/
        iterator finish;/*指向最后一个节点。*/
        map_pointer map;/*指向一块map区域，map内都是指针，指向一个缓冲区。*/
        size_type map_size;/*map内的指针数量*/
        enum{ __initial_map_size = 8};

    public:
        /*constructor*/
        deque() { __fill_initialize(size_type(),T());}
        deque(size_type n,const T& value){ __fill_initialize(n,value);}
        explicit deque(size_type n){ __fill_initialize(n,T());}
        template<class InputIterator>
        deque(InputIterator first,InputIterator last);

        /*copy constructor*/
        deque(const deque& x);

        /*assignment operation*/
        deque& operator=(const deque& x);

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
        const_reverse_iterator rend() { return reverse_iterator(begin());}

        /*about container operation*/
        bool empty(){ return begin() == end();}
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
        template<class InputIterator>
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
        iterator __reverse_elements_at_back(size_type n);
        iterator __reverse_elements_at_front(size_type n);
        void __reverse_map_at_back(size_type n = 1);
        void __reverse_map_at_front(size_type n = 1);
        void __reallocate_map(size_type node_to_add,bool add_at_front);
    };


/*constructor*/
template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
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

/*assignment operator*/
template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>& deque<T,Alloc,BufSize>::operator=(const deque& x){
    size_type len = size();
    if(*this != x){
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
template<class InputIterator>
void deque<T,Alloc,BufSize>::assign(InputIterator first,InputIterator last){
    typedef typename __is_integer<InputIterator>::is_integer integer;
    __assign_dispatch(first,last,integer());
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
    difference_type index = position - start;
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
        difference_type n = last - first;
        difference_type elem_before = first - start;
        if(elem_before < (size() - n) / 2){
            mjstl::copy_backward(start,first,last);
            iterator new_start = start + n;
            data_allcator::destory(start,new_start);
            /*释放缓冲区，是否必要？*/
            for(map_pointer cur = start.node; cur != new_start.node; ++cur)
                data_allcator::deallocate(*cur,buffer_size());
            start = new_start;
        }else{
            mjstl::copy(last,finish,first);
            iterator new_finish = finish - n;
            data_allcator::destory(new_finish,finish);
            /*finish也在具体缓冲块中，不能直接删除。*/
            for(map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                data_allcator::deallocate(*cur,buffer_size());
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
        data_allcator::destory(*cur,*cur + buffer_size());
        data_allcator::deallocate(*cur,buffer_size());
    }

    /*分情况释放start和finish缓冲区的元素。*/
    /*如果不在同一块*/
    if(start.node != finish.node){
        data_allcator::destory(start.cur,start.last);
        data_allcator::destory(finish.first,finish.cur);
        data_allcator::deallocate(*(finish.node),buffer_size());
    }else{
        data_allcator::destory(start.cur,finish.cur);
    }
    finish = start;
}

/*insert an element at the end of container*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::push_back(const T& x){
    if(finish.cur != finish.last - 1){
        data_allcator::construct(finish.cur,x);
        ++finish.cur;
    }else
        __push_back_aux(x);
}

/*insert an element at the begin of container*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::push_front(const T& x){
    if(start.cur != start.first){
        /*这里应该先自减1再进行构造？*/
        --start.cur;
        data_allcator::construct(start.cur,x);
    }else
        __push_front_aux(x);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::pop_back(){
    if(finish.cur != finish.first){
        --finish.cur;
        data_allcator::destory(finish.cur);
    }else
        __pop_back_aux();
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::pop_front(){
    /*左闭右开，这里start.cur是不可能为start.last的，最多start.last-1。*/
    if(start.cur != start.last - 1){
        data_allcator::destory(start.cur);
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
        insert(finish,len - new_size,x);
}

/*swap*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::swap(deque& x){
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
            *cur = data_allcator::allocate(buffer_size());
    }catch(...){
        __destory_node(nstart,nfinish);
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__destory_node(map_pointer nstart,map_pointer nfinish){
    /*这里释放内存还保留了最后一个缓冲块*/
    for(map_pointer n = nstart; n < nfinish; ++n)
        data_allcator::deallocate(*n,buffer_size());
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
    map_pointer nfinish = map + nNode - 1;
    /*
    *   当nNode >= 6 时，map_size >= 8; 
    *       (map_size - nNode) = 2; nNode - 1为最后一个。
    *   当nNode < 6时，map_size = 8;
    *       2 < (map_size - nNode) <= 7; 
    * */
    try{
        __create_node(nstart,nfinish);
    }catch(...){
        map_allocator::deallocator(map,map_size);
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
    }
}

template<class T,class Alloc,size_t BufSize>
template<class InputIterator>
void deque<T,Alloc,BufSize>::__range_initialize(InputIterator first,
    InputIterator last,input_iterator_tag){
    __map_initialize(0);
    try{
        /*这里因为输入是单向迭代器，只能一个一个进行初始化。*/
        for(; first != last; ++first)
            push_back(*first);
    }catch(...){
        clear();
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
    InputIterator last,__true_type){
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
    difference_type index = position - start;
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
    difference_type elements_before = position - start;
    size_type len = size();
    value_type x_copy = x;
    if(elements_before < difference_type(len/2)){
        iterator old_start = start;
        iterator new_start = __reverse_map_at_front(n);
        position = start + elements_before;

        try{
            if(elements_before >= difference_type(n)){
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
        iterator new_finish = __reverse_elements_at_back(n);
        const difference_type elements_after = difference_type(len) - elements_before;
        position = finish - elements_after;

        try{
            /*插入点后的元素大于插入元素个数n。*/
            if(elements_after > difference_type(n)){
                iterator finish_n = finish - difference_type(n);
                mjstl::uninitialized_copy(finish_n,finish,finish);
                finish = new_finish;
                mjstl::copy_backward(position,finish_n,old_finish);
                mjstl::fill(position,position + difference_type(n),x_copy);
            }else{
                /*插入点后的元素少于n。
                *   1、[finish,position + n)可以直接uninitialized_fill;
                *   2、使用uninitialized_copy移动[position,old_finish) 
                *       到 [position + n,new_finish)；
                *   3、在使用fill填充步骤2移走的区域[position,old_finish)
                */
                iterator finish_n = position + difference_type(n);
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
    difference_type elements_before = position - start;
    size_type len = size();
    if(elements_before < difference_type(len/2)){
        iterator old_start = start;
        iterator new_start = __reverse_map_at_front(n);
        position = start + elements_before;
        try{
            if(elements_before >= difference_type(n)){
                iterator start_n = position + difference_type(n);
                mjstl::uninitialized_copy(start,start_n,new_start);
                start = new_start;
                mjstl::copy(start_n,position,old_start);
                mjstl::copy(first,last,position - difference_type(n));
            }else{
                ForwardIterator mid = first;
                advance(mid,difference_type(n) - elements_before);
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
        iterator new_finish = __reverse_elements_at_back(n);
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
                iterator mid = first;
                advance(first,difference_type(n) - elements_after);
                mjstl::uninitialized_copy(position,finish,
                    mjstl::uninitialized_copy(mid,last,finish));
                finish = new_finish;
                mjstl::copy(first,mid,position);
            }
        }catch(...){
            __destory_node(finish.node + 1,new_finish.node + 1)
        }
    }
}

template<class T,class Alloc,size_t BufSize>
template<class Integer>
void deque<T,Alloc,BufSize>::__insert_dispatch(iterator position,
    Integer n,Integer value,__true_type){
    __fill_insert(position,n,x);
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
    mjstl::copy(first,last,iterator(*this,position))
}

template<class T,class Alloc,size_t BufSize>
template<class ForwardIterator>
void deque<T,Alloc,BufSize>::__insert_dispatch_aux(iterator position,
    ForwardIterator first,ForwardIterator last,forward_iterator_tag){
    size_type n = distance(first,last);
    if(position.cur = start.cur){
        iterator new_start = __reverse_elements_at_front(n);
        try{
            mjstl::uninitialized_copy(first,last,new_start);
            start = new_start;
        }catch(...){
            __destory_node(new_start.node,start.node);
        }
    }else if(position.cur == finish.cur){
        iterator new_finish = __reverse_elements_at_back(n);
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
    __reverse_map_at_back();
    *(finish.node + 1) = data_allcator::allocate(__deque_buf_size(sizeof(T)));
    try{
        data_allcator::construct(finish.cur,x_copy);
        finish.set_node(finish.node + 1);
        finish.cur = finish.first;
    }catch(...){
        data_allcator::deallocate(*(finish.node + 1));
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__push_front_aux(const T& x){
    value_type x_copy = x;
    __reverse_map_at_front();
    *(start.node - 1) = data_allcator::allocate(__deque_buf_size(sizeof(T)));
    try{
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        data_allcator::construct(start.cur,x_copy);
    }catch(...){
        ++start;
        data_allcator::deallocate(*(start.node - 1));
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__pop_back_aux(){
    data_allocator::deallocate(finish.first);
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    data_allcator::destory(finish.cur);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__pop_front_aux(){
    ata_allcator::destory(start.cur);
    data_allocator::deallocate(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::__reverse_elements_at_back(size_type n){
    /*finish.last - finish.cur - 1为什么要多减1？？*/
    if(n > (size_type)(finish.last - finish.cur - 1)){
        /*使其至少为1*/
        size_type new_node = (n + buffer_size() - 1) / buffer_size();
        __reverse_map_at_back(new_node);

        size_type i;
        try{
            for(i = 1; i <= new_node; ++i)
                *(finish.node + i) = data_allcator::allocate(__deque_buf_size(sizeof(T)));
        }catch(...){
            for(size_type j = 1; j < i; ++j)
                data_allcator::deallocate(finish.node + j);
        }
    }
    return finish + difference_type(n);
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator 
deque<T,Alloc,BufSize>::__reverse_elements_at_front(size_type n){
    /*原本写的start.cur - start.last 我认为不对啊。*/
    if(n > (size_type)(start.cur - start.first)){
        /*使其至少为1*/
        size_type new_node = (n + buffer_size() - 1) / buffer_size();
        __reverse_map_at_front(new_node);

        size_type i;
        try{
            for(i = 1; i <= new_node; ++i)
                *(start.node - i) = data_allcator::allocate(__deque_buf_size(sizeof(T)));
        }catch(...){
            for(size_type j = 1; j < i; ++j)
                data_allcator::deallocate(start.node - j);
        }
    }
    return start - difference_type(n);
}

/*如果不够就节点就分配。*/
template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__reverse_map_at_back(size_type n){
    if(n + 1 > map_size - (finish.node - map))
        __reallocate_map(n,false);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::__reverse_map_at_front(size_type n){
    if(n + 1 > start.node - map)
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
            mjstl::copy_backward(start.node,finish.node + 1,new_start + old_nodes_num)
    }else{
        size_type new_map_size = map_size + mjstl::map(map_size,node_to_add) + 2;
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