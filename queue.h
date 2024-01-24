#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "heap_algo.h"
#include "../deque.h"

namespace mjstl
{
template<class T,class Container = mjstl::deque<T>>
class queue
{
public:
    typedef typename Container::value_type              value_type;
    typedef typename Container::size_type               size_type;
    typedef typename Container::reference               reference;
    typedef typename Container::const_reference         const_reference;

    static_assert(std::is_same<T,value_type>::value,
        "the value_type of Container should be same with T.");

public:
    typedef Container   container_type;

private:
    Container   c_;
public:
    queue() = default;
    queue(size_type n)
      :c_(n){};

    queue(size_type n,const T& value)
      :c_(n,value){};

    template<class Iter>
    queue(Iter first,Iter last)
      :c_(first,last){}

    queue(std::initializer_list<value_type> ilist)
      :c_(ilist.begin(),ilist.end()){};

    queue(const Container& c)
      :c_(c){}
    
    queue(Container&& c)
      :c_(std::move(c)){}

    queue(const queue& rhs)
      :c_(rhs.c_){};

    queue(queue&& rhs)
      :c_(std::move(rhs.c_)){};
    
    queue& operator=(const queue& rhs)
    {
        c_ = rhs.c_;
        return *this;
    }

    queue& operator=(queue&& rhs)
    {
        c_ = std::move(rhs.c_);
        return *this;
    }

    ~queue() = default;

public:
    reference       front()         { return c_.front(); }
    const_reference front() const   { return c_.front(); }
    reference       back()          { return c_.back(); }
    const_reference back()  const   { return c_.back(); }

    bool        empty() const noexcept { return c_.empty(); }
    size_type   size()  const noexcept { return c_.size(); }

    template<class ...Args>
    void emplace(Args&&... args)
    {
        c_.emplace_back(std::forward<Args>(args)...);
    }

    void push(const value_type& value)
    {
        c_.push_back(value);
    }

    void push(value_type&& value)
    {
        c_.emplace_back(std::move(value));
    }

    void pop()
    {
        c_.pop_front();
    }

    void clear()
    {
        while(!c_.empty())
            c_.pop_front();
    }

    void swap(queue& rhs) noexcept(noexcept(mjstl::swap(c_,rhs.c_)))
    {
        mjstl::swap(c_,rhs.c_);
    }
};

template<class T,class Container>
bool operator==(const queue<T,Container>& lhs,const queue<T,Container>& rhs)
{
    return lhs == rhs;
}

template<class T,class Container>
bool operator!=(const queue<T,Container>& lhs,const queue<T,Container>& rhs)
{
    return !(lhs == rhs);
}

template<class T,class Container>
bool operator<(const queue<T,Container>& lhs,const queue<T,Container>& rhs)
{
    return lhs < rhs;
}

template<class T,class Container>
bool operator<=(const queue<T,Container>& lhs,const queue<T,Container>& rhs)
{
    return !(rhs < lhs);
}

template<class T,class Container>
bool operator>(const queue<T,Container>& lhs,const queue<T,Container>& rhs)
{
    return rhs < lhs;
}

template<class T,class Container>
bool operator>=(const queue<T,Container>& lhs,const queue<T,Container>& rhs)
{
    return !(lhs < rhs);
}

template<class T,class Container>
void swap(queue<T,Container>& lhs,queue<T,Container>& rhs)
{
    lhs.swap(rhs);
}

template<class T,class Container = mjstl::deque<T>,
  class Compare = std::less<typename Container::value_type>>
class priority_queue
{
public:
    typedef typename    Container::value_type       value_type;
    typedef typename    Container::size_type        size_type;
    typedef typename    Container::reference        reference;
    typedef typename    Container::const_reference  const_reference;
public:
    typedef Container   container_type;
    typedef Compare     value_compare;
private:
    container_type  c_;
    value_compare comp_;

public:
    priority_queue() = default;

    priority_queue(const Compare c)
      :c_(),comp_(c){}

    explicit priority_queue(size_type n)
      :c_(n)
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }
    
    priority_queue(size_type n,const value_type& value)
      :c_(n,value)
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    template<class IIter>
    priority_queue(IIter first,IIter last)
      :c_(first,last)
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    priority_queue(std::initializer_list<value_type> ilist)
      :c_(ilist.begin(),ilist.end())
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    priority_queue(const Container& c)
      :c_(c)
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    priority_queue(Container&& c)
      :c_(std::move(c))
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    priority_queue(const priority_queue& rhs)
      :c_(rhs.c_),comp_(rhs.comp_)
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    priority_queue(priority_queue&& rhs)
      :c_(std::move(rhs.c_)),comp_(rhs.comp_)
    {
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
    }

    priority_queue& operator=(const priority_queue& rhs)
    {
        c_ = rhs.c_;
        comp_ = rhs.comp_;
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
        return *this;
    }

    priority_queue& operator=(priority_queue&& rhs)
    {
        c_ = std::move(rhs.c_);
        comp_ = std::move(rhs.comp_);
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
        return *this;
    }

    priority_queue& operator=(std::initializer_list<value_type> ilist)
    {
        c_ = ilist;
        comp_ = value_compare();
        mjstl::make_heap(c_.begin(),c_.end(),comp_);
        return *this;
    }

    ~priority_queue() = default;

public:
    const_reference top() const { return c_.front(); }

    bool        empty()     const noexcept { return c_.empty(); }
    size_type   size()      const noexcept { return c_.size(); }

    template<class... Args>
    void emplace(Args&& ...args)
    {
        c_.emplace_back(std::forward<Args>(args)...);
        mjstl::push_heap(c_.begin(),c_.end(),comp_);
    }

    void push(const value_type& value)
    {
        c_.push_back(value);
        mjstl::push_heap(c_.begin(),c_.end(),comp_);
    }

    void push(value_type&& value)
    {
        c_.push_back(std::move(value));
        mjstl::push_heap(c_.begin(),c_.end(),comp_);
    }

    void pop()
    {
        mjstl::pop_head(c_.begin(),c_.end(),comp_);
        c_.pop_back();
    }

    void clear()
    {
        while(!c_.empty())
            pop();
    }

    void swap(priority_queue& rhs) noexcept(noexcept(mjstl::swap(c_,rhs.c_))
                                        && noexcept(mjstl::swap(comp_,rhs.comp_)))
    {
        mjstl::swap(c_,rhs.c_);
        mjstl::swap(comp_,rhs.comp_);
    }

public:
    friend bool operator==(const priority_queue& lhs,const priority_queue& rhs)
    {
        return lhs.c_ == rhs.c_;
    }

    friend bool operator!=(const priority_queue& lhs,const priority_queue& rhs)
    {
        return lhs.c_ != rhs.c_;
    }
};


template<class T,class Container,class Compare>
bool operator==(const priority_queue<T,Container,Compare>& lhs,
                const priority_queue<T,Container,Compare>& rhs)
{
    return lhs == rhs;
}

template<class T,class Container,class Compare>
bool operator!=(const priority_queue<T,Container,Compare>& lhs,
                const priority_queue<T,Container,Compare>& rhs)
{
    return lhs != rhs;
}

template<class T,class Container,class Compare>
void swap(priority_queue<T,Container,Compare>& lhs,
          priority_queue<T,Container,Compare>& rhs)
{
    lhs.swap(rhs);
}

} // namespace mjstl

#endif// !__QUEUE_H__