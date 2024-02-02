#ifndef __LIST_H__
#define __LIST_H__

#include "iterator.h"
#include "reverse_iterator.h"
#include "memory.h"

namespace mjstl
{
    /*list node*/
    template<class T>
    struct __list_node{
        T data;
        __list_node<T>* prev;
        __list_node<T>* next;

        __list_node():data(0),prev(nullptr),next(nullptr){}

        __list_node(const T& value,__list_node<T>* p = nullptr,
            __list_node<T>* n = nullptr):data(value),prev(p),next(n){}
    };

    /*iteartor*/
    template<class T,class Ref,class Ptr>
    struct __list_iterator : public iterator<bidirectional_iterator_tag,T>{
        typedef __list_iterator<T,T&,T*> iterator;
        typedef __list_iterator<T,const T&,const T*> const_iterator;
        typedef __list_iterator<T,Ref,Ptr> self;

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef __list_node<T>* link_type;

        link_type node;

        /*constructor*/
        __list_iterator():node(nullptr){}
        __list_iterator(link_type x):node(x){}
        __list_iterator(const iterator& x):node(x.node){}

        /*overload operator*/
        bool operator==(const self& x) const{ return node == x.node;}
        bool operator!=(const self& x) const{ return node != x.node;}
        reference operator*() const { return (*node).data;}
        pointer operator->() const { return &(operator*());}
        self& operator++(){
            node = node->next;
            return *this;
        }

        self operator++(int){
            self tmp = *this;
            node = node->next;
            return tmp;
        }

        self& operator--(){
            node = node->prev;
            return *this;
        }

        self operator--(int){
            self tmp = *this;
            node = node->prev;
            return tmp;
        }
    };

    template<class T,class Alloc = simple_alloc<__list_node<T>>>
    class list{
    public:
        typedef T                       value_type;
        typedef Alloc                   allocate_type;
        typedef value_type*             pointer;
        typedef const value_type*       const_pointer;
        typedef value_type&             reference;
        typedef const value_type&       const_reference;
        typedef size_t                  size_type;
        typedef ptrdiff_t               difference_type;

        typedef __list_iterator<T,T&,T*>                iterator;
        typedef __list_iterator<T,const T&,const T*>    const_iterator;
        typedef mjstl::reverse_iterator<iterator>              reverse_iterator;
        typedef mjstl::reverse_iterator<const_iterator>        const_reverse_iterator;

    public:
        typedef mjstl::allocator<__list_node<T>> data_allocator;
    public:
        typedef __list_node<T>* link_type;

    protected:
        link_type node;
        size_type size_;
    public:
        list(){ __initialize();}
        explicit list(size_type n);
        explicit list(size_type n,const T& value);
        list(std::initializer_list<value_type> ilist);
        template<class InputIterator,typename std::enable_if<
            mjstl::is_input_iterator<InputIterator>::value,int>::type = 0>
        list(InputIterator first,InputIterator last);

        /*copy constructor*/
        list(const list& x);
        list(list&& x);

        /*assignment operator*/
        list& operator=(const list& x);
        list& operator=(list&& x);
        list& operator=(std::initializer_list<value_type> ilist);
        /*destructor*/
        ~list();

    public:
        /*about iterator*/
        iterator begin(){ return iterator(node->next);}
        const_iterator begin() const { return iterator(node->next);}
        iterator end(){ return iterator(node);}
        const_iterator end() const { return node;}
        reverse_iterator rbegin(){ return reverse_iterator(end());}
        const_reverse_iterator rbegin() const { return reverse_iterator(end());}
        reverse_iterator rend(){ return reverse_iterator(begin());}
        const_reverse_iterator rend() const { return reverse_iterator(begin());}

        /*about container*/
        reference front(){ return *begin();}
        const_reference front() const { return *begin();}
        reference back(){ return *(--end());}
        const_reference back() const { return *(--end());}
        bool empty() { return begin() == end();}
        size_type size() const { return size_;}

        /*assign container*/
        void assign(size_type n,const T& value){ __fill_assign(n,value);}
        void assign(size_type n){ __fill_assign(n,T());}
        template<class InputIterator,typename std::enable_if<
            mjstl::is_input_iterator<InputIterator>::value,int>::type = 0>
        void assign(InputIterator first,InputIterator last);
        iterator insert(iterator position,const T& x);
        iterator insert(iterator position){ return insert(position,T());}
        void insert(iterator position,size_type n,const T& value);
        template<class InputIterator,typename std::enable_if<
            mjstl::is_input_iterator<InputIterator>::value,int>::type = 0>
        void insert(iterator position,InputIterator first,InputIterator last);
        iterator erase(iterator position);
        iterator erase(iterator first,iterator last);
        void clear();
        void push_front(const T& x){ insert(begin(),x);}
        void push_front(){ insert(begin());}
        void push_back(const T& x){ insert(end(),x);}
        void push_back(){ insert(end());}
        void pop_front(){ erase(begin());}
        void pop_back(){ auto tmp = end(); erase(--tmp);}
        void resize(size_type new_size,const T& x);
        void resize(size_type new_size){ return resize(new_size,T());}
        void swap(list& x){ mjstl::swap(node,x.node); mjstl::swap(size_,x.size_); }

        /*container operation*/
        void splice(iterator position,list& x);
        void splice(iterator position,list&,iterator i);
        void splice(iterator position,list& x,iterator first,iterator last);
        void remove(const T& value);
        template<class Predicate>
        void remove_if(Predicate pred);
        void unique();
        void merge(list& x);
        void sort();
        void reverse();

        /*about allocator*/
        allocate_type get_allcate(){ return allocate_type();}

    protected:
        link_type __create_node(const T& x = T());
        void __destory_node(link_type p);
        void __initialize();
        void __fill_assign(size_type n,const T& value);
        template<class Integer>
        void __assign_dispatch(Integer n,Integer value,__true_type);
        template<class InputIterator>
        void __assign_dispatch(InputIterator first,InputIterator last,__false_type);
        void __fill_insert(iterator position,size_type n,const T& value);
        template<class Integer>
        void __insert_dispatch(iterator position,Integer n,Integer value,__true_type);
        template<class InputIterator>
        void __insert_dispatch(iterator position,InputIterator first,
            InputIterator last,__false_type);
        void __transfer(iterator position,iterator first,iterator last);
    };

template<class T,class Alloc>
list<T,Alloc>::list(size_type n){
    __initialize();
    insert(begin(),n,T());
}

template<class T,class Alloc>
list<T,Alloc>::list(size_type n,const T& value){
    __initialize();
    insert(begin(),n,value);
}

template<class T,class Alloc>
list<T,Alloc>::list(std::initializer_list<value_type> ilist){
    __initialize();
    insert(begin(),ilist.begin(),ilist.end());
}

template<class T,class Alloc>
template<class InputIterator,typename std::enable_if<
  mjstl::is_input_iterator<InputIterator>::value,int>::type>
list<T,Alloc>::list(InputIterator first,InputIterator last){
    __initialize();
    insert(begin(),first,last);
}

template<class T,class Alloc>
list<T,Alloc>::list(const list<T,Alloc>& x){
    __initialize();
    insert(begin(),x.begin(),x.end());
}

template<class T,class Alloc>
list<T,Alloc>::list(list<T,Alloc>&& x)
  :node(x.node),size_(x.size_)
{
    x.node = nullptr;
    x.size_ = 0;
}


template<class T,class Alloc>
list<T,Alloc>& list<T,Alloc>::operator=(const list<T,Alloc>& x){
    if(this != &x){
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        for(;first1 != last1 && first2 != last2; ++first1,++first2)
            *first1 = *first2;
        if(first2 == last2)
            erase(first1,last1);
        else
            insert(first1,first2,last2);
    }
    return *this;
}

template<class T,class Alloc>
list<T,Alloc>& list<T,Alloc>::operator=(list<T,Alloc>&& x){
    if(this != &x){
        list<T,Alloc> tmp(std::move(x));
        swap(tmp);
    }
    return *this;
}

template<class T,class Alloc>
list<T,Alloc>::~list(){
    if(node == nullptr) return;
    link_type first = node->next;
    link_type last = node;
    while(first != last){
        link_type cur = first;
        first = first->next;
        __destory_node(cur);
    }
    __destory_node(node);
    node = nullptr;
}

template<class T,class Alloc>
template<class InputIterator,typename std::enable_if<
  mjstl::is_input_iterator<InputIterator>::value,int>::type>
void list<T,Alloc>::assign(InputIterator first,InputIterator last){
    __assign_dispatch(first,last,__false_type());
}

/* position位置插入一个元素x。*/
template<class T,class Alloc>
typename list<T,Alloc>::iterator 
list<T,Alloc>::insert(iterator position,const T& x){
    link_type tmp = __create_node(x);
    tmp->next = (link_type)(position.node);
    tmp->prev = (link_type)(position.node->prev);
    //position.node->prev = tmp;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    ++size_;
    return tmp;
}

/*在position位置插入n个元素x。*/
template<class T,class Alloc>
void list<T,Alloc>::insert(iterator position,size_type n,const T& x){
    __fill_insert(position,n,x);
}

/*在position位置插入[first,last)个元素。*/
template<class T,class Alloc>
template<class InputIterator,typename std::enable_if<
    mjstl::is_input_iterator<InputIterator>::value,int>::type>
void list<T,Alloc>::insert(iterator position,InputIterator first,InputIterator last){
    __insert_dispatch(position,first,last,__false_type());
}

/* 删除position处的元素。*/
template<class T,class Alloc>
typename list<T,Alloc>::iterator 
list<T,Alloc>::erase(iterator position){
    link_type pprev = position.node->prev;
    link_type pnext = position.node->next;
    pprev->next = pnext;
    pnext->prev = pprev;
    __destory_node(position.node);
    --size_;
    return (iterator)pnext;
}

/*删除[first,last)的元素。*/
template<class T,class Alloc>
typename list<T,Alloc>::iterator 
list<T,Alloc>::erase(iterator first,iterator last){
    while(first != last)
        erase(first++);
    return last;
}

/*重置容器大小。*/
template<class T,class Alloc>
void list<T,Alloc>::resize(size_type new_size,const T& x){
    iterator it = begin();
    size_type len = 0;
    while(it != end() && len < new_size){
        ++it;
        ++len;
    }

    /*len == new_size说明容器到it之前已经够new_size了，后面都删掉。*/
    if(len == new_size)
        erase(it,end());
    else/*说明到尾了，大小仍然不够。*/
        insert(end(),new_size - len,x);
}

/*清空*/
template<class T,class Alloc>
void list<T,Alloc>::clear(){
    link_type first = (link_type)node->next;
    link_type last = (link_type)node;
    while(first != last){
        link_type cur = first;
        first = first->next;
        __destory_node(cur);
    }
    size_ = 0;
    node->prev = node->next = node;
}

/*将 list x接合于position之前。*/
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator position,list& x){
    if(!x.empty())
        __transfer(position,x.begin(),x.end());
    size_ += x.size_();
    x.size_ = 0;
}

/* 将it所指元素接和于position之前。*/
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator position,list& x,iterator it){
    iterator jit = it;
    ++jit;
    if(position == it || position == jit) return;
    /* 其实就是[it,it+1)*/
    __transfer(position,it,jit);
    --x.size_;
    ++size_;
}

/* 将[first,last)元素接和于position之前。*/
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator position,list& x,iterator first,iterator last){
    if(first == last) return;
    size_type len = mjstl::distance(first,last);
     __transfer(position,first,last);
     x.size_ -= len;
     size_  += len;
}

/*将数值所有为value的元素移除*/
template<class T,class Alloc>
void list<T,Alloc>::remove(const T& value){
    iterator first = begin();
    iterator last = end();
    while(first != last){
        iterator next = first;
        ++next;
        if(*first == value) erase(first);
        first = next;
    }
}

/*使用一元操作pred，将使得pred为true的元素移除*/
template<class T,class Alloc>
template<class Predicate>
void list<T,Alloc>::remove_if(Predicate pred){
    iterator first = begin();
    iterator last = end();
    while(first != last){
        iterator next = first;
        ++next;
        if(pred(*first)) erase(first);
        first = next;
    }
}

/*移除连续且相同的元素。*/
template<class T,class Alloc>
void list<T,Alloc>::unique(){
    iterator first = begin();
    iterator last = end();
    if(first == last) return ;
    iterator next = first;
    while(++next != last){
        if(*next == *first)
            erase(next);
        else
            first = next;
        /*原next可能已经失效，得重新回到first*/
        next = first;
    }
}

/*与链表x合并*/
template<class T,class Alloc>
void list<T,Alloc>::merge(list<T,Alloc>& x){
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while(first1 != last1 && first2 != last2){
        if(*first2 < *first1){
            iterator next = first2;
            __transfer(first1,first2,++next);
            first2 = next;
        }else
            ++first1;
    }
    if(first2 != last2)
        __transfer(last1,first2,last2);
    size_ += x.size();
    x.size_ = 0;
}

template<class T,class Alloc>
void list<T,Alloc>::sort(){
    list<T,Alloc> carry;
    list<T,Alloc> counter[64];
    int fill = 0;
    while(!empty()){
        carry.splice(carry.begin(),*this,begin());
        int i = 0;

        /*
        *   相当于二进制进位，1个数放counter[0],2个数counter[1],4个数counter[2]...
        * 一旦i == fill，那么意味着进位，元素个数是1，2，4，8，16，32，64...
        */
        while(i < fill && !counter[i].empty()){
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }

        /*如果counter[0]空，那么会直接来到这一步。直接插入counter[0]。*/
        carry.swap(counter[i]);
        /*相当于扩充二进制位数。*/
        if(i == fill) ++fill;
    }

    /*在依次合并*/
    for(int i = 1; i < fill; ++i){
        counter[i].merge(counter[i-1]);
    }
    /*最后一个就是合并好的链表，交换回来。*/
    swap(counter[fill-1]);
}

template<class T,class Alloc>
void list<T,Alloc>::reverse(){
    if(node == node->next || (link_type)node->next->next == node) return;
    iterator first = begin();
    ++first;
    /*不断的把first+1后面的元素重新进行头插入。*/
    while(first != end()){
        iterator old = first;
        ++first;
        __transfer(begin(),old,first);
    }
}

template<class T,class Alloc>
typename list<T,Alloc>::link_type 
list<T,Alloc>::__create_node(const T& x){
    link_type p = data_allocator::allocate();
    try{
        data_allocator::construct(p,__list_node<T>(x));
    }catch(...){
        data_allocator::deallocate(p);
    }
    return p;
}

template<class T,class Alloc>
void list<T,Alloc>::__destory_node(link_type p){
    mjstl::destory(&p->data);
    data_allocator::deallocate(p);
}

template<class T,class Alloc>
void list<T,Alloc>::__initialize(){
    node = __create_node();
    node->prev = node->next = node;
    size_ = 0;
}

template<class T,class Alloc>
void list<T,Alloc>::__fill_assign(size_type n,const T& value){
    iterator it = begin();
    for(; it != end() && n > 0; ++it,--n)
        *it = value;
    
    if(n == 0)
        erase(it,end());
    else
        insert(it,n,value);
}

template<class T,class Alloc>
template<class Integer>
void list<T,Alloc>::__assign_dispatch(Integer n,Integer value,__true_type){
    __fill_assign((size_type)n,(T)value);
}

template<class T,class Alloc>
template<class InputIterator>
void list<T,Alloc>::__assign_dispatch(InputIterator first2,InputIterator last2,__false_type){
    iterator first1 = begin();
    iterator last1 = end();
    for(; first1 != last1 && first2 != last2; ++first1,++first2)
        *first1 = *first2;
    if(first2 != last2)
        insert(first1,first2,last2);
    else
        erase(first1,last1);
}

template<class T,class Alloc>
void list<T,Alloc>::__fill_insert(iterator position,size_type n,const T& value){
    for(; n > 0; --n)
        insert(position,value);
}

template<class T,class Alloc>
template<class Integer>
void list<T,Alloc>::__insert_dispatch(iterator position,Integer n,Integer value,__true_type){
    __fill_insert(position,(size_type)n,(T)value);
}

template<class T,class Alloc>
template<class InputIterator>
void list<T,Alloc>::__insert_dispatch(iterator position,InputIterator first,
    InputIterator last,__false_type){
    for(;first != last; ++first)
        insert(position,*first);
}

template<class T,class Alloc>
void list<T,Alloc>::__transfer(iterator position,iterator first,iterator last){
    if(position == last) return;
    /*
    *   草率用 endit = --last; 后面还需要last不妥。
    * 然后想到last不能变用后缀--，endit = last--，这样
    * endit就不是last的前一个结点了，没作用。写糊涂了。
    * 只能用 endit = last.node->prev;
    */
    iterator endit = last.node->prev;
    /*先把[first,last)取下来，再缝合first前面后last元素。*/
    first.node->prev->next = (link_type)last.node;
    last.node->prev = (link_type)first.node->prev;
    /*将[first,last)插入position前面*/
    endit.node->next = (link_type)position.node;
    first.node->prev = (link_type)position.node->prev;
    first.node->prev->next = (link_type)first.node;
    position.node->prev = (link_type)endit.node;
}

template<class T,class Alloc>
inline bool operator==(const list<T,Alloc>& x,const list<T,Alloc>& y){
    typedef typename list<T,Alloc>::const_iterator const_iterator;
    const_iterator first1 = x.begin();
    const_iterator last1 = x.end();
    const_iterator first2 = y.begin();
    const_iterator last2 = y.end();
    for(;first1 != last1 && first2 != last2; ++first1,++first2);
    return first1 == last1 && first2 == last2;
}

template<class T,class Alloc>
inline bool operator<(const list<T,Alloc>& x,const list<T,Alloc>& y){
    return mjstl::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
}

template<class T,class Alloc>
inline bool operator>(const list<T,Alloc>& x,const list<T,Alloc>& y){
    return y < x;
}

template<class T,class Alloc>
inline bool operator<=(const list<T,Alloc>& x,const list<T,Alloc>& y){
    return !(y < x);
}

template<class T,class Alloc>
inline bool operator>=(const list<T,Alloc>& x,const list<T,Alloc>& y){
    return !(x < y);
}

template<class T,class Alloc>
inline void swap(list<T,Alloc>& x,list<T,Alloc>& y){
    x.swap(y);
}
} // namespace mjstl
#endif// !__LIST_H__