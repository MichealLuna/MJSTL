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


    template<class T>
    struct has_iterator_cat{
    private:
        struct two{ char a; char b;};

        template<class U> static two test(...);
        template<class U> static char test(typename U::iterator_category* = 0);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*,typename Reference = T&>
    struct iterator{
        typedef  Category      iterator_category;
        typedef  T             value_type;
        typedef  Distance      distance_type;
        typedef  Pointer       pointer;
        typedef  Reference     reference;
    };

    template<class Iterator,bool>
    struct iterator_traits_imp{};

    template<class Iterator>
    struct iterator_traits_imp<Iterator,true>{
        typedef typename Iterator::iterator_category    iterator_category;
        typedef typename Iterator::value_type           value_type;
        typedef typename Iterator::difference_type      difference_type;
        typedef typename Iterator::pointer              pointer;
        typedef typename Iterator::reference            reference;
    };

    template<class Iterator,bool>
    struct iterator_traits_helper{};

    template<class Iterator>
    struct iterator_traits_helper<Iterator,true> 
        : public iterator_traits_imp<Iterator,
        std::is_convertible<typename Iterator::iterator_category,input_iterator_tag>::value ||
        std::is_convertible<typename Iterator::iterator_category,output_iterator_tag>::value>{};

    template<class Iterator>
    struct iterator_traits : public iterator_traits_helper<Iterator,
        has_iterator_cat<Iterator>::value>{};
    template<class T>
    struct iterator_traits<T*>{
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };
    template<class T>
    struct iterator_traits<const T*>{
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef const T*                    pointer;
        typedef const T&                    reference;
    };

    /*
    *   第一个是参数类型T，第二参数相当于一个实参。
    * 用来实例化这个类共有的value值。
    */
    template<class T,T v>
    struct m_integeral_constant{
        static constexpr T value = v;
    };
    /*
    *   实例化true，false
    */
    template<bool b>
    using m_bool_constant = m_integeral_constant<bool,b>;

    typedef m_bool_constant<true> m_true_type;
    typedef m_bool_constant<false> m_false_type;

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
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        return static_cast<difference_type*>(0);
    }

    /*value_type*/
    template <typename Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&){
        typedef typename iterator_traits<Iterator>::value_type value_type;
        return static_cast<value_type*>(0);
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

    template<class T,class U,bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of : public m_bool_constant<std::is_convertible<
        typename iterator_traits<T>::iterator_category,U>::value>
    {};

    template<class T,class U>
    struct has_iterator_cat_of<T,U,false> : public m_false_type{};

    template<class T>
    struct is_input_iterator : public has_iterator_cat_of<T,input_iterator_tag>{};

    template<class T>
    struct is_output_iterator : public has_iterator_cat_of<T,output_iterator_tag>{};

    template<class T>
    struct is_forward_iterator : public has_iterator_cat_of<T,forward_iterator_tag>{};

    template<class T>
    struct is_bidirectional_iterator : public has_iterator_cat_of<T,bidirectional_iterator_tag>{};

    template<class T>
    struct is_random_access_iterator : public has_iterator_cat_of<T,random_access_iterator_tag>{};

    template<class Iter>
    struct is_iterator : public m_bool_constant<is_input_iterator<Iter>::value ||
        is_output_iterator<Iter>::value>{};

} // namespace mjstl
#endif// !__ITERATOR_H__