#ifndef __STACK_H__
#define __STACK_H__

#include "deque.h"

namespace mjstl
{

template<class T,class Container = mjstl::deque<T>>
class stack
{
public:
    typedef     Container       container_type;

public:
    typedef typename    Container::value_type       value_type;
    typedef typename    Container::size_type        size_type;
    typedef typename    Container::reference        reference;
    typedef typename    Container::const_reference  const_reference;

    static_assert(std::is_same<T,value_type>::value,
        "the value_type of Container should be same with T.");

private:
    container_type  c_;

public:
    stack() = default;

    stack(size_type n)
      :c_(n){}

    stack(size_type n,const T& value)
      :c_(n,value){}
    
    template<class IIter>
    stack(IIter first,IIter last)
      :c_(first,last){}

    stack(std::initializer_list<value_type> ilist)
      :c_(ilist.begin(),ilist.end()){}

    stack(const Container& c)
      :c_(c){}

    stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
      :c_(std::move(c)){}

    stack(const stack& rhs)
      :c_(rhs.c_){}

    stack(stack&& rhs)
      :c_(std::move(rhs.c_)){}

    stack& operator=(const stack& rhs)
    {
        /*这里不做判断this == &rhs，交给容器判断。*/
        c_ = rhs.c_;
        return *this;
    }

    stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
    {
        c_ = std::move(rhs.c_);
        return *this;
    }

    stack& operator=(std::initializer_list<value_type> ilist)
    {
        c_ = ilist;
        return *this;
    }

    ~stack() = default;

    reference        top()           { return c_.back(); }
    const_reference  top() const     { return c_.back(); }

    bool        empty()const noexcept { return c_.empty(); }
    size_type   size() const noexcept { return c_.size(); }

    // template<class... Args>
    // void emplace(Args&& ...args)
    // { c_.emplace_back(mjstl::forward<Args>(arg)...); } 

    void push(const value_type& value)
    { c_.push_back(value); }
    void push(value_type&& value)
    { c_.push_back(std::move(value)); }
    
    void pop()
    { c_.pop_back(); }

    void clear()
    {
        while(!c_.empty())
            c_.pop_back();
    }

    void swap(stack& rhs) noexcept(noexcept(mjstl::swap(c_,rhs.c_)))
    { mjstl::swap(c_,rhs.c_); }

public:
    friend bool operator==(const stack& lhs,const stack& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator<(const stack& lhs,const stack& rhs) { return lhs.c_ < rhs.c_; }
};

template<class T,class Container>
bool operator==(const stack<T,Container>& lhs,const stack<T,Container>& rhs)
{
    return lhs == rhs;
}

template<class T,class Container>
bool operator<(const stack<T,Container>& lhs,const stack<T,Container>& rhs)
{
    return lhs < rhs;
}

template<class T,class Container>
bool operator!=(const stack<T,Container>& lhs,const stack<T,Container>& rhs)
{
    return !(lhs == rhs);
}

template<class T,class Container>
bool operator>(const stack<T,Container>& lhs,const stack<T,Container>& rhs)
{
    return rhs < lhs;
}

template<class T,class Container>
bool operator<=(const stack<T,Container>& lhs,const stack<T,Container>& rhs)
{
    return !(rhs < lhs);
}

template<class T,class Container>
bool operator>=(const stack<T,Container>& lhs,const stack<T,Container>& rhs)
{
    return !(lhs < rhs);
}

/*提供一个stack的swap的版本。overload mjstl::swap*/
template<class T,class Container>
void swap(stack<T,Container>& lhs,stack<T,Container>& rhs)
{
    lhs.swap(rhs);
}

} // namespace mjstl
#endif// !__STACK_H__