#ifndef __STACK_TEST_H__
#define __STACK_TEST_H__

#include <stack>
#include "../stack.h"
#include "test.h"

namespace mjstl
{
namespace test
{
namespace stack_test
{

void print_stack(mjstl::stack<int> st){
    while(!st.empty()){
        std::cout<<" "<<st.top();
        st.pop();
    }
    std::cout<<std::endl;
}

#define STACK_COUT(s) do{                   \
    std::string s_name = #s;                \
    std::cout<<" "<<s_name<<" :";           \
    print_stack(s);                         \
}while(0)

#define STACK_AFTER_FUN(con,fun) do{        \
    std::string s_fun = #fun;               \
    std::cout<<"After "<<s_fun<<std::endl;  \
    fun;                                    \
    STACK_COUT(con);                        \
}while(0)


void stack_test(){
    std::cout<<"[===============================================================]"<<std::endl;
    std::cout<<"[----------------- Run container test : deque ------------------]"<<std::endl;
    std::cout<<"[---------------------------API test----------------------------]"<<std::endl;

    int a[] = {1,2,3,4,5,6,7,8};
    mjstl::deque<int> d1(a,a + 8);
    mjstl::stack<int> s1;
    mjstl::stack<int> s2(10);
    mjstl::stack<int> s3(10,1);
    mjstl::stack<int> s4(a,a + sizeof(a)/sizeof(int));
    mjstl::stack<int> s5{1,2,3,4,5,6,7,8};
    mjstl::stack<int> s6(d1);
    mjstl::stack<int> s8(std::move(d1));
    mjstl::stack<int> s9(s2);
    mjstl::stack<int> s10(std::move(s2));
    mjstl::stack<int> s11;
    s11 = s3;
    mjstl::stack<int> s12;
    s12 = std::move(s3);
    mjstl::stack<int> s13 = {1,2,3,4,5,6,7,8};

    STACK_AFTER_FUN(s1,s1.push(1024));
    STACK_AFTER_FUN(s1,s1.push(512));
    STACK_AFTER_FUN(s1,s1.push(256));
    STACK_AFTER_FUN(s1,s1.push(128));
    STACK_AFTER_FUN(s1,s1.push(64));
    STACK_AFTER_FUN(s1,s1.push(32));
    STACK_AFTER_FUN(s1,s1.push(16));
    STACK_AFTER_FUN(s1,s1.push(8));
    STACK_AFTER_FUN(s1,s1.push(4));
    STACK_AFTER_FUN(s1,s1.push(2));
    STACK_AFTER_FUN(s1,s1.push(1));
    STACK_AFTER_FUN(s1,s1.pop());
    STACK_AFTER_FUN(s5,s5.clear());
    STACK_AFTER_FUN(s6,s6.swap(s9));

    while(!s13.empty()){
        STACK_AFTER_FUN(s13,s13.pop());
    }
    PASSED;

#if PERFORMANCE_TEST_ON
    std::cout<<"[--------------------- Performance Testing ---------------------]"<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|        push         |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(stack<int>,push,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(stack<int>,push,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|--------------|------------|"<<std::endl;
    PASSED;
#endif// PERFORMANCE_TEST_ON
    std::cout<<"[----------------- End container test : stack ------------------]"<<std::endl;
}

} // namespace stack_test
} // namespace mjstl
} // namespace test
#endif// !__STACK_TEST_H__