#ifndef __REVERSE_ITERATOR_H__
#define __REVERSE_ITERATOR_H__

namespace ZMJ{

template<class Iterator>
class reverse_iterator{
protected:
    Iterator current;

public:
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type value_type;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::pointer pointer;
    typedef typename iterator_traits<Iterator>::reference reference;

    typedef Iterator iterator_type; /*正向迭代器*/
    typedef reverse_iterator<Iterator> self; /*反向迭代器*/

public:
    reverse_iterator(){}
    explicit reverse_iterator(iterator_type x):current(x){}
    reverse_iterator(const self& x):current(x.current){}

    iterator_type base() const{
        return current;
    }

    reference operator*() const{
        Iterator tmp = current; /*实际对应正向迭代器前一个位置。*/
        return *--tmp;
    }

    pointer operator->()const{
        return &(operator*());
    }

    /*前进变后退*/
    self& operator++(){
        --current;
        return *this;
    }

    self operator++(int){
        self tmp = *this;
        --current;
        return *tmp;
    }

    /*后退变前进*/
    self& operator--(){
        ++current;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        ++current;
        return tmp;
    }

    self operator-(difference_type n) const{
        return self(current + n);
    }

    self operator+(difference_type n) const{
        return self(current - n);
    }

    self operator-=(difference_type n) {
        current = current + n;
        return *this;
    }

    self operator+=(difference_type n) {
        current -=n;
        return *this;
    }

    reference operator[](difference_type n) const{
        return *(*this + n);
    }
};

}//namespace ZMJ
#endif//!__REVERSE_ITERATOR_H__