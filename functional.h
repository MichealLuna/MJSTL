#ifndef __FUNCTION_H__
#define __FUNCTION_H__

namespace mjtest
{
    template<class Arg,class Result>
    struct unarg_function{
        typedef Arg     argument_type;
        typedef Result  result_type;
    };

    template<class Arg1,class Arg2,class Result>
    struct binary_function{
        typedef Arg1     first_argument_type;
        typedef Arg2     second_argument_type;
        typedef Result   result_type;
    };

    template<class T>
    struct plus : public binary_function<T,T,T>{
        T operator()(const T& x,const T& y) const { return x + y; }
    };

    template<class T>
    struct minus : public binary_function<T,T,T>{
        T operator()(const T& x,const T& y) const { return x - y; }
    };

    template<class T>
    struct multiplies : public binary_function<T,T,T>{
        T operator()(const T& x,const T& y) const { return x * y; }
    };

    template<class T>
    struct divides : public binary_function<T,T,T>{
        T operator()(const T& x,const T& y) const { return x / y; }
    };

    template<class T>
    struct modulus : public binary_function<T,T,T>{
        T operator()(const T& x,const T& y) const { return x % y; }
    };

    template<class T>
    struct negate : public unarg_function<T,T>{
        T operator()(const T& x,const T& y) const { return -x; }
    };

    /*
    *   加法的证同元素
    */
    template<class T>
    inline T indentity_element(plus<T>){ return T(0); }

    /*
    *   乘法的证同元素
    */
    template<class T>
    inline T indentity_element(multiplies<T>){ return T(1); }

    template<class T>
    struct equal_to : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x == y; }
    };

    template<class T>
    struct not_equal_to : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x != y; }
    };

    template<class T>
    struct greater : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x > y; }
    };

    template<class T>
    struct less : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x < y; }
    };

    template<class T>
    struct greater_equal : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x >= y; }
    };

    template<class T>
    struct less_equal : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x <= y; }
    };

    template<class T>
    struct logical_and : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x && y; }
    };

    template<class T>
    struct logical_or : public binary_function<T,T,bool>{
        bool operator()(const T& x,const T& y) const { return x || y; }
    };

    template<class T>
    struct logical_not : public unarg_function<T,bool>{
        bool operator()(const T& x) const { return !x; }
    };

    /*证同元素： 不会改变元素，返回本身。*/
    template<class T>
    struct identity : public unarg_function<T,T>{
        const T& operator()(const T& x) const { return x; }
    };

    /*
    *   选择函数：接受一个pair，返回第一个元素。
    */
    template<class Pair>
    struct selectfirst : public unarg_function<Pair,typename Pair::first_type>
    {
        const typename Pair::first_type& operator()(const Pair& x) const{
            return x.first;
        }
    };
    
    /*
    *   选择函数：接受一个pair，返回第二个元素。
    */
    template<class Pair>
    struct selectsecond : public unarg_function<Pair,typename Pair::second_type>
    {
        const typename Pair::second_type& operator()(const Pair& x) const{
            return x.second;
        }
    };

    /*
    *   投射函数： 返回第一个参生
    */
    template<class Arg1,class Arg2>
    struct projectfirst : public binary_function<Arg1,Arg2,Arg1>{
        Arg1 operator()(const Arg1& x,const Arg2& y) const{ return x; }
    };

    /*
    *   投射函数： 返回第二个参生
    */
    template<class Arg1,class Arg2>
    struct projectfirst : public binary_function<Arg1,Arg2,Arg1>{
        Arg2 operator()(const Arg1& x,const Arg2& y) const{ return y; }
    };

} // namespace mjtest
#endif// !__FUNCTION_H__