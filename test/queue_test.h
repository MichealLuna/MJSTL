#ifndef __QUEUE_TEST_H__
#define __QUEUE_TEST_H__

#include <queue>
#include "../queue.h"
#include "test.h"

namespace mjstl
{
namespace test
{
namespace queue_test
{
    
void queue_print(mjstl::queue<int> q)
{
    while(!q.empty())
    {
        std::cout<<" "<<q.front();
        q.pop();
    }
    std::cout<<std::endl;
}

void priority_queue_print(mjstl::priority_queue<int> p)
{
    while(!p.empty())
    {
        std::cout<<" "<<p.top();
        p.pop();
    }
    std::cout<<std::endl;
}

#define QUEUE_COUT(q) do{               \
    std::string qname = #q;             \
    std::cout<<" "<<qname<<" :";        \
    queue_print(q);                     \
}while(0)

#define PQUEUE_COUT(p) do{              \
    std::string pname = #p;             \
    std::cout<<" "<<pname<<" :";        \
    priority_queue_print(p);            \
}while(0)

#define QUEUE_AFTER_FUN(con,fun) do{    \
    std::string fun_name = #fun;        \
    std::cout<<" "<<fun_name<<" :\n";   \
    fun;                                \
    QUEUE_COUT(con);                    \
}while(0)

#define PQUEUE_AFTER_FUN(con,fun) do{   \
    std::string fun_name = #fun;        \
    std::cout<<" "<<fun_name<<" :\n";   \
    fun;                                \
    PQUEUE_COUT(con);                   \
}while(0)

void queue_test()
{

    std::cout<<"[===============================================================]"<<std::endl;
    std::cout<<"[----------------- Run container test : queue ------------------]"<<std::endl;
    std::cout<<"[---------------------------API test----------------------------]"<<std::endl;

    int a[] = {1,2,3,4,5,6,7,8};
    mjstl::deque<int> d1(a,a+sizeof(a)/sizeof(int));
    mjstl::queue<int> q1;
    mjstl::queue<int> q2(10,101);
    mjstl::queue<int> q3(a,a+sizeof(a)/sizeof(int));
    mjstl::queue<int> q4{1,2,3,4,5,6,7,8,9,10};
    mjstl::queue<int> q5(d1);
    mjstl::queue<int> q6(std::move(d1));
    mjstl::queue<int> q7(q2);
    mjstl::queue<int> q8(std::move(q2));
    mjstl::queue<int> q9 = q3;
    mjstl::queue<int> q10 = std::move(q3);

    int lval = 1000;
    std::cout<<std::boolalpha;
    QUEUE_AFTER_FUN(q10,q10.emplace(10));
    QUEUE_AFTER_FUN(q10,q10.push(100));
    QUEUE_AFTER_FUN(q10,q10.push(lval));
    QUEUE_AFTER_FUN(q10,q10.pop());
    QUEUE_AFTER_FUN(q10,q10.swap(q4));
    FUN_VALUE(q10.empty());
    QUEUE_AFTER_FUN(q10,q10.clear());
    FUN_VALUE(q10.size());
    FUN_VALUE(q10.empty());
    std::cout<<std::noboolalpha;
    PASSED;
#if PERFORMANCE_TEST_ON
    std::cout<<"[--------------------- Performance Testing ---------------------]"<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|        push         |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(queue<int>,push,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(queue<int>,push,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    PASSED;
#endif
    std::cout<<"[------------------ End container test : queue -----------------]"<<std::endl;
}

void priority_queue_test()
{
    std::cout<<"[===============================================================]"<<std::endl;
    std::cout<<"[--------------Run container test : priority_queue -------------]"<<std::endl;
    std::cout<<"[---------------------------API test----------------------------]"<<std::endl;

    int a[] = {1,2,3,4,5,6,7,8};
    mjstl::deque<int> d1(a,a+sizeof(a)/sizeof(int));
    mjstl::priority_queue<int> q1;
    mjstl::priority_queue<int> q2(10,101);
    mjstl::priority_queue<int> q3(a,a+sizeof(a)/sizeof(int));
    mjstl::priority_queue<int> q4{1,2,3,4,5,6,7,8,9,10};
    mjstl::priority_queue<int> q5(d1);
    mjstl::priority_queue<int> q6(std::move(d1));
    mjstl::priority_queue<int> q7(q2);
    mjstl::priority_queue<int> q8(std::move(q2));
    mjstl::priority_queue<int> q9 = q3;
    mjstl::priority_queue<int> q10 = std::move(q3);

    int lval = 1000;
    std::cout<<std::boolalpha;
    PQUEUE_AFTER_FUN(q10,q10.emplace(10));
    PQUEUE_AFTER_FUN(q10,q10.push(100));
    PQUEUE_AFTER_FUN(q10,q10.push(lval));
    PQUEUE_AFTER_FUN(q10,q10.pop());
    PQUEUE_AFTER_FUN(q10,q10.swap(q4));
    FUN_VALUE(q10.empty());
    PQUEUE_AFTER_FUN(q10,q10.clear());
    FUN_VALUE(q10.size());
    FUN_VALUE(q10.empty());
    std::cout<<std::noboolalpha;

#if PERFORMANCE_TEST_ON
    std::cout<<"[--------------------- Performance Testing ---------------------]"<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|        push         |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(priority_queue<int>,push,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(priority_queue<int>,push,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    PASSED;
#endif
    std::cout<<"[------------- End container test : priority_queue -------------]"<<std::endl;
}

} // namespace queue_test
} // namespace test
} // namespace mjstl
#endif// !__QUEUE_TEST_H__