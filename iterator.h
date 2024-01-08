#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#ifndef USE_CSTDDEF
#define USE_CSTDDEF
#include <cstddef>
#endif // !USE_CSTDDEF
namespace mjstl
{
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag{};
    struct random_access_iterator_tag : public bidirectional_iterator_tag{};

    template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*,typename Reference = T&>
    class itreator{
        using iterator_category     = Category;
        using value_type            = T;
        using difference_type       = Distance;
        using pointer               = Pointer;
        using reference             = Reference;
    };

    template <typename T>
    struct iterator_traits{
        using iterator_category     =  typename T::iterator_category;
        using value_type            =  typename T::value_type;
        using difference_type       =  typename T::difference_type;
        using pointer               =  typename T::pointer;
        using reference             =  typename T::reference;
    };

    template<typename T>
    struct iterator_traits<T*>{
        using iterator_category     =  random_access_iterator_tag;
        using value_type            =  T;
        using difference_type       =  ptrdiff_t;
        using pointer               =  T*;
        using reference             =  T&;
    };

    template<typename T>
    struct iterator_traits<const T*>{
        using iterator_category     =  random_access_iterator_tag;
        using value_type            =  T;
        using difference_type       =  ptrdiff_t;
        using pointer               =  const T*;
        using reference             =  const T&;
    };

    /*iterator_category*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&){
        using category = typename iterator_traits<Iterator>::iterator_category;
        return category();
    }

    /*distance_type*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&){
        using difference_type = typename iterator_traits<Iterator>::difference_type;
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    /*value_type*/
    template <typename Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&){
        using value_type = typename iterator_traits<Iterator>::value_type;
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    /*以上为什么返回value_type* 和 difference_type* ?  对于value_type*,应该是迭代器维护对象的原生指针。
    * 返回指针是可以理解的，因为如果返回实际对象，可能对时间空间浪费极大，而且返回指针类型也不担心，因为有type_traits。
    * 但对于表示迭代器的距离distance_type,为什么也要difference_type*?（可能difference_type有时不是一个整型）
    * 1、约定
    * 2、零成本抽象，空指针指针。
    * 3、不用产生实例化对象。
    */
    template<typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last){
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return __distance(first,last,category());
    }

    template<typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag){
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while(first != last){
            ++first;
            ++n;
        }
        return n;
    }

    template<typename RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    __distance(RandomAccessIterator first,RandomAccessIterator last,random_access_iterator_tag){
        return last - first;
    }

    template<typename InputIterator,class Distance>
    inline void advance(InputIterator& i,Distance n){
        __advance(i,n,iterator_category(i));
    }

    template<typename InputIterator,class Distance>
    inline void __advance(InputIterator& i,Distance n,input_iterator_tag){
        while(n > 0){
            ++i;
            --n;
        }
    }
    
    template<typename InputIterator,class Distance>
    inline void __advance(InputIterator& i,Distance n,bidirectional_iterator_tag){
        if(n >= 0){
            while(n > 0){
                --n;
                ++i;
            }
        }else{
            while(n < 0){
                ++n;
                ++i;
            }
        }
    }

    template<typename RandonAccessIterator,class Distance>
    inline void __advance(RandonAccessIterator& i,Distance n,random_access_iterator_tag){
        i += n;
    }

} // namespace mjstl
#endif// !__ITERATOR_H__