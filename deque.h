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
        typedef allocator<T*Alloc> map_allocator;

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
        iterator insert_aux(iterator position,const T& x);
        void __fill_insert(iterator position,size_type n,const T& x);
        void __fill_insert_aux(iterator position,size_type n,const T& x);
        template<class ForwardIterator>
        void __fill_insert_aux(iterator,ForwardIterator,ForwardIterator,size_type);
        template<class Integer>
        void __insert_dispatch(iterator,Integer,Integer,__true_type);
        template<class InputIterator>
        void __insert_dispatch(InputIterator first,InputIterator last,__false_type);
        template<class InputIterator>
        void __insert_dispatch_aux(InputIterator,InputIterator,input_iterator_tag);
        template<class ForwardIterator>
        void __insert_dispatch_aux(ForwardIterator,ForwardIterator,forward_iterator_tag);
        void __push_back_aux(const T& x);
        void __push_front_aux(const T& x);
        void __pop_back_aux();
        void __pop_front_aux();
        iterator __reverse_elements_at_back(size_type n);
        iterator __reverse_elements_at_front(size_type n);
        void __reverse_map_at_back(size_type n);
        void __reverse_map_at_front(size_type n);
        void __reallocate_map(size_type node_to_add,bool add_at_front);
    };
} // namespace ZMJ
#endif// !__DEQUE_H__