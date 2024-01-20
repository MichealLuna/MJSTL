#ifndef __QUEUE_H__
#define __QUEUE_H__

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
public:
    typedef Container   container_type;
private:
    Container   c_;
public:
    queue() = default;
    queue(size_type n):c_(n){};
    queue(size_type n,const T& value):c_(n,value){};
    queue(std::initializer_list<value_type> ilist):_c(ilist){};
    queue(const queue& rhs):c_(rhs.c_){};
    queue(queue&& rhs):c_(std::move(rhs.c_)){};
    
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
    reference front() { return c_.front(); }
    const_reference front() const noexcept { return c_.front(); }
    reference back() { return c_.back(); }
    const_reference back() const noexcept { return c_.back(); }

    bool empty() { return c_.empty(); }
    size_type size() { return c_.size(); }

    void push(const value_type& value)
    {
        c_.push_back(value);
    }

    void push(value_type&& value)
    [
        c_.push_back(std::move(value));
    ]

    void pop()
    {
        c_.pop_front();
    }
};
} // namespace mjstl

#endif// !__QUEUE_H__