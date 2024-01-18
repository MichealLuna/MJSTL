#ifndef __MJSTL_DEQUE_TEST_H__
#define __MJSTL_DEQUE_TEST_H__

#include <deque>
#include "../deque.h"

#include "test.h"

namespace mjstl{
namespace test{
namespace deque_test{

void deque_test(){
    std::cout<<"[===============================================================]"<<std::endl;
    std::cout<<"[----------------- Run container test : deque ------------------]"<<std::endl;
    std::cout<<"[---------------------------API test----------------------------]"<<std::endl;
    int a[] = {1,2,3,4,5};
    mjstl::deque<int> d1;
    mjstl::deque<int> d2(22);
    mjstl::deque<int> d3(33,1);
    mjstl::deque<int> d4(a,a+5);
    mjstl::deque<int> d5(d2);
    mjstl::deque<int> d6(std::move(d2));
    mjstl::deque<int> d7;
    d7 = d3;
    mjstl::deque<int> d8;
    d8 = std::move(d3);
    mjstl::deque<int> d9{1,2,3,4,5,6,7,8,9,10};
    mjstl::deque<int> d10 = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    FUN_AFTER(d1,d1.assign(5,1));
    FUN_AFTER(d1,d1.assign(8,8));
    FUN_AFTER(d1,d1.assign(a,a+5));
    FUN_AFTER(d1,d1.assign({1,2,3,4,5}));
    FUN_AFTER(d1,d1.insert(d1.end(),6));
    FUN_AFTER(d1,d1.insert(d1.end() - 1,2,7));
    FUN_AFTER(d1,d1.insert(d1.begin(),a,a+5));
    FUN_AFTER(d1,d1.erase(d1.begin()));
    FUN_AFTER(d1,d1.erase(d1.begin(),d1.begin() + 4));
    FUN_AFTER(d1,d1.push_front(1));
    FUN_AFTER(d1,d1.push_back(2));
    FUN_AFTER(d1,d1.pop_back());
    FUN_AFTER(d1,d1.pop_front());
    FUN_AFTER(d1,d1.resize(5));
    FUN_AFTER(d1,d1.resize(1024,8));
    FUN_AFTER(d1,d1.clear());
    FUN_AFTER(d1,d1.swap(d4));
    FUN_AFTER(d4,d4.size());
    FUN_VALUE(*(d1.begin()));
    FUN_VALUE(*(d1.end() - 1));
    FUN_VALUE(*(d1.rbegin()));
    FUN_VALUE(*(d1.rend() - 1));
    FUN_VALUE(d1.front());
    FUN_VALUE(d1.back());
    FUN_VALUE(d1.at(1));
    FUN_VALUE(d1[2]);

    std::cout<< std::boolalpha;
    FUN_VALUE(d1.empty());
    std::cout<< std::noboolalpha;
    FUN_VALUE(d1.size())
    FUN_VALUE(d1.max_size());
    PASSED;

#if PERFORMANCE_TEST_ON
    std::cout<<"[--------------------- Performance Testing ---------------------]"<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|     push_front      |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(deque<int>,push_front,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(deque<int>,push_front,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|     push_back       |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(deque<int>,push_front,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(deque<int>,push_back,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    PASSED;
#endif
    std::cout<<"[----------------- End container test : deque ------------------]"<<std::endl;
}

/*
*   debug日志：调试了很多bug，有个bug让我找了1天。
* 误用了全局函数__deque_buf_size(size_t,size_t)，实际应该使用deque内部的
* 函数buffer_size()。__deque_buf_size（512，sizeof(T)），表示缓冲区大小为512，
* 而buffer_size()调用了__deque_buf_size,分配内存块时使用buffer_size()表示缓冲区
* 大小。但我使用成__deque_buf_size(n,sizeof(T)),这里n是分配的元素个数，结果分配的
* 内存块就n个字节。
*  
* 因为分配的过于小的内存是跟二级分配器分配的，所以这个分配的内存是在二级分配器，而不是
*直接malloc。问题就在于结束释放内存时，正常释放，把之前分配的n个字节当成了正常缓冲块大小，
*由于正常缓冲块大小为512字节，它选择free进行释放，而不是二级分配器释放。
*   这就造成了，在二级分配器申请内存，而却找1级分配器释放内存，结果就是释放不存在的内存。
* 
*
*   找bug历程：
* 1、根据报错发现free失败，那么就猜想是哪个地方多次释放了。
* 2、接着就一个一个d1 ~ d10十个容器一个一个找，最后发现是释放跟d1容器交换后d4出了问题。
* 然而，我仔细跟踪了构造函数、移动构造函数、或者swap，或者__destroy_node，发现都没问题。
* 陷入死局，哪哪儿都没错。
* 3、以上碰壁了很多次，最后突然想到一个比较笨的办法，就是注释其中一半影响d1的代码，这样
* 就可以每次缩小一半的范围，最后找到一个地方d1.assign这个函数调用了，结果就导致崩溃。
* 4、接着就细细跟踪assign，一路跟踪到__reverse_elements_at_front函数里面发现了上述的
* 问题。
*
*/


}// namespace deque_test
} // namespace test
}// namespace mjstl
#endif // !__MJSTL_DEQUE_TEST_H__