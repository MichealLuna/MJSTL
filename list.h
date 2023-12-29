#ifndef __LIST_H__
#define __LIST_H__

#include "iterator.h"
#include "reverse_iterator.h"
#include "memory.h"

namespace ZMJ
{
    /*list node*/
    template<class T>
    struct __list_node{
        T data;
        __list_node<T>* prev;
        __list_node<T>* next;

        __list_node():data(0),prev(nullptr),next(nullptr){}

        __list_node(T value = 0,__list_node<T>* p,
            __list_node<T>* n):data(value),prev(p),next(n){}
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

        typedef __list_node<T>* link_type;

        link_type node;

        /*constructor*/
        __list_iterator(){}
        __list_iterator(link_type x):node(x){}
        __list_iterator(const iterator& x):node(x.node){}

        /*overload operator*/
        bool operator==(const self& x) const{ return node == x.node;}
        bool operator!=(const self& x) const{ return node != x.node;}
        reference operator*() const { return (*node).data;}
        pointer operator->() const { return &((*node).data);}
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
        typedef reverse_iterator<iterator>              reverse_iterator;
        typedef reverse_iterator<const_iterator>        const_reverse_iterator;

    public:
        typedef Alloc       data_allocate;
    public:
        typedef __list_node<T>* link_type;

    protected:
        link_type node;
    public:
        list(){ __initialize();}
        explicit list(size_type n);
        explicit list(size_type n,const T& value);
        template<class InputIterator>
        list(InputIterator first,InputIterator last);

        /*copy constructor*/
        list(const list& x);

        /*assignment operator*/
        list& operator=(const list& x);

        /*destructor*/
        ~list();

    public:
        /*about iterator*/
        iterator begin(){ return (link_type)node->next;}
        const_iterator begin(){ return (link_type)node->next;}
        iterator end(){ return node;}
        const_iterator end(){ return node;}
        reverse_iterator rbegin(){ return reverse_iterator(end());}
        const_reverse_iterator rbegin(){ return reverse_iterator(end());}
        reverse_iterator rend(){ return reverse_iterator(begin());}
        const_reverse_iterator rend(){ return reverse_iterator(begin());}

        /*about container*/
        reference front(){ return *begin();}
        const_reference front(){ return *begin();}
        reference back(){ return *(--end());}
        const_reference back(){ return *(--end());}
        bool empty() { return begin() == end();}

        /*assign container*/
        void assign(size_type n,const T& value){ __fill_assign(n,value);}
        void assign(size_type n){ __fill_assign(n,T());}
        template<class InputIterator>
        void assign(InputIterator first,InputIterator last);
        iterator insert(iterator position,const T& x);
        iterator insert(iterator position){ return insert(position,T());}
        void insert(iterator position,size_type n,const T& value);
        template<class InputIterator>
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
        void swap(list& x){ ZMJ::swap(node,x.node);}

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
template<class InputIterator>
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
list<T,Alloc>::~list(){
    link_type first = begin();
    link_type last = end();
    while(first != last){
        link_type cur = first;
        first = first->next;
        __destory_node(cur);
    }
}

template<class T,class Alloc>
template<class InputIterator>
void list<T,Alloc>::assign(InputIterator first,InputIterator last){
    typedef typename __is_integer<InputIterator>::is_integer is_integer;
    __assign_dispatch(first,last,is_integer());
}

/* position位置插入一个元素x。*/
template<class T,class Alloc>
typename list<T,Alloc>::iterator 
list<T,Alloc>::insert(iterator position,const T& x){
    link_type tmp = __create_node(x);
    tmp->next = (link_type)position.node;
    tmp->prev = (link_type)position.node->prev;
    position.node->prev = tmp;
    position.node->prev->next = tmp;
    return tmp;
}

/*在position位置插入n个元素x。*/
template<class T,class Alloc>
void list<T,Alloc>::insert(iterator position,size_type n,const T& x){
    __fill_insert(position,n,value);
}

/*在position位置插入[first,last)个元素。*/
template<class T,class Alloc>
template<class InputIterator>
void list<T,Alloc>::insert(iterator position,InputIterator first,InputIterator last){
    typedef typename __is_integer<InputIterator>::is_integer integer;
    __insert_dispatch(position,first,last,integer());
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
        insert(it,new_size - len,x);
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
    node->prev = node->next = node;
}

/*将 list x接合于position之前。*/
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator position,list& x){
    if(!x.empty())
        __transfer(position,x.begin(),x.end());
}

/* 将it所指元素接和于position之前。*/
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator position,list&,iterator it){
    iterator jit = it;
    ++jit;
    if(position == it || position == jit) return;
    /* 其实就是[it,it+1)*/
    __transfer(position,it,jit);
}

/* 将[first,last)元素接和于position之前。*/
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator position,list& x,iterator first,iterator last){
    if(first != last)
        __transfer(position,first,last);
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
        __transfer(first1,first2,last2);
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
    link_type p = data_allocate::allocate();
    try{
        data_allocate::construct(p,__list_node<T>(x));
    }catch(...){
        data_allocate::deallocate(p);
    }
    return p;
}

template<class T,class Alloc>
void list<T,Alloc>::__destory_node(link_type p){
    ZMJ::destory(&p->data);
    data_allocate::deallocate(p);
}

template<class T,class Alloc>
void list<T,Alloc>::__initialize(){
    node = __create_node();
    node->prev = node->next = node;
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
    for(; first1 != last1 && first2 != last2; ++first,++first1)
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
    iterator endit = --last;
    /*先把[first,last)取下来，再缝合first前面后last元素。*/
    (link_type)first.node->prev = (link_type)last.node;
    (link_type)last.node->prev = (link_type)first.node->prev;
    /*将[first,last)插入position前面*/
    (link_type)endit.node->next = (link_type)position.node;
    (link_type)first.node->prev = (link_type)position.node->prev;
    (link_type)first.node->prev->next = (link_type)first.node;
    (link_type)position.node->prev = (link_type)last.node->prev
}
} // namespace ZMJ
#endif// !__LIST_H__