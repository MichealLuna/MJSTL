#ifndef __TEMPBUF_H__
#define __TEMPBUF_H__ 

#ifndef USE_CSTDDEF
#define USE_CSTDDEF
#include <cstddef>
#endif// !USE_CSTDDEF

#include "type_traits.h"
#include "uninitialized.h"
namespace ZMJ
{
    template<class T>
    pair<T*,ptrdiff_t> get_temporary_buffer(ptrdiff_t len){
        return __get_temporary_buffer(len,T*(0));
    }

    template<class T>
    pair<T*,ptrdiff_t> get_temporary_buffer(ptrdiff_t len,T*){
        return __get_temporary_buffer(len,T*(0));
    }

    template<class T>
    pair<T*,ptrdiff_t> __get_temporary_buffer(ptrdiff_t len,T*){
        if(len > ptrdiff_t(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);

        while(len > 0){
            T* tmp = (T*)malloc(size_t(len) * sizeof(T));
            if(tmp != 0)
                return pair<T*,ptrdiff_t>(tmp,len);
            len /= 2;/*申请失败减少1倍。*/
        }
        return pair<T*,ptrdiff_t>((T*)0,0);
    }   

    template<class T>
    void return_temporary_buffer(T* ptr){
        free(ptr);
    }

    template<class ForwardIterator,class T>
    class temporary_buffer{
    private:
        ptrdiff_t original_len;
        ptrdiff_t len;
        T* buffer;

        void allocate_buffer(){
            original_len = len;
            if(len > (ptrdiff_t)INT_MAX / sizeof(T))
                len = INT_MAX / sizeof(T);
            while(len > 0){
                buffer = (T*)malloc(len * sizeof(T));
                if(buffer != 0)
                    break;
                len /= 2;
            }
        }

        void initialize_buffer(const T&,__true_type){
            
        }

        void initialize_buffer(const T& value,__false_type){
            uninitialized_fill_n(buffer,len,value);
        }

        void inintialize_buffer(ForwardIterator first,ForwardIterator last,__false_type){
            uninitialized_copy(first,last,buffer);
        }

    public:
        temporary_buffer(ForwardIterator first,ForwardIterator last){
            typedef typename __type_traits<T>::has_trivial_default_constructor trivial;
            try{
                len = distance(first,last);
                allocate_buffer();
                if(len > 0){
                    //initialize_buffer(*first,trivial());
                    initialize_buffer(first,last,trivial());
                }
            }catch(...){
                free(buffer);
                buffer = 0;
                len = 0;
            }
        }

        ~temporary_buffer(){
            destory(buffer,buffer + len);
            free(buffer);
        }

        ptrdiff_t size() const { return len;}
        ptrdiff_t requested_size() const { return original_len;}
        T* begin(){ return buffer;}
        T* end(){ return buffer + len;}
    
    private:
        temporary_buffer(const temporary_buffer&){}
        void operator=(const temporary_buffer&){}
    };
} // namespace ZMJ
#endif//!__TEMPBUF_H__