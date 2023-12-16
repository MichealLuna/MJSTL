#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <stddef.h>

namespace ZMJ
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
        using iterator_category     = typename T::iterator_category;
        using value_type            = typename T::value_type;
        using difference_type       = typename T::difference_type;
        using pointer               = typename T::pointer;
        using reference             = typename T::reference;
    };

    template<typename T>
    struct iterator_traits<T*>{
        using iterator_category     = typename ramdom_access_iterator_tag;
        using value_type            = typename T;
        using difference_type       = typename ptrdiff_t;
        using pointer               = typename T*;
        using reference             = typename T&;
    };

    template<typename T>
    struct iterator_traits<const T*>{
        using iterator_category     = typename ramdom_access_iterator_tag;
        using value_type            = typename T;
        using difference_type       = typename ptrdiff_t;
        using pointer               = typename const T*;
        using reference             = typename const T&;
    };

    /*iterator_category*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&){
        using category = iterator_traits<Iterator>::iterator_category;
        return category();
    }

    /*distance_type*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&){
        using difference_type = iterator_traits<Iterator>::difference_type;
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    /*value_type*/
    template <typename Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&){
        using value_type = iterator_traits<Iterator>::value_type;
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    /*以上为什么返回value_type* 和 difference_type* ?  对于value_type*,应该是迭代器维护对象的原生指针。
    * 返回指针是可以理解的，因为如果返回实际对象，可能对时间空间浪费极大，而且返回指针类型也不担心，因为有type_traits。
    * 但对于表示迭代器的距离distance_type,为什么也要difference_type*?（可能difference_type有时不是一个整型）
    * 1、约定
    * 2、零成本抽象，空指针指针。
    * 3、不用产生实例化对象。
    */
    

} // namespace ZMJ



#endif//iterator.h