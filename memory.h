#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "algobase.h"
#include "allocator.h"
#include "sgi_allocator.h"
#include "uninitialized.h"
#include "tempbuf.h"

namespace ZMJ
{
    template<class T>
    class auto_ptr{
    private:
        T* m_ptr;
    public:
        typedef T elem_type;

    public:
        explicit auto_ptr(T* p = 0):m_ptr(0){}
        auto_ptr(auto_ptr& x):m_ptr(x.release()){}
        
        template<class U>
        auto_ptr(auto_ptr<T>& x):m_ptr(x.release()){}

        /*赋值前会把原来的资源给释放掉。*/
        auto_ptr& operator=(auto_ptr& x){
            if(&x != this){
                delete m_ptr;
                m_ptr = x.release();
            }
            return *this;
        }

        template<class U>
        auto_ptr<U>& operator=(auto_ptr<U>& x){
            if(x.get() != this->get()){
                delete m_ptr;
                m_ptr = x.release();
            }
        }

        /*auto_ptr只管理堆上的内容，不包括栈，全局变量。*/
        ~auto_ptr(){
            delete m_ptr;
        }
    public:
        T& operator*() const{
            return *m_ptr;
        }

        T* operator->() const{
            return m_ptr;
        }

        T* get() const{
            return m_ptr;
        }

        /*m_ptr不做delete，而是传出去。*/
        T* release(){
            T* tmp = m_ptr;
            m_ptr = 0;
            return tmp;
        }

        /*reset会delete原来的资源，重新设置为p。*/
        void reset(T* p){
            if(p != m_ptr){
                delete m_ptr;
                m_ptr = p;
            }
        }
    };
} // namespace ZMJ
#endif//!__MEMORY_H__