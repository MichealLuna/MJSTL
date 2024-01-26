#ifndef __VECTOR_TEST_H__
#define __VECTOR_TEST_H__

#include <vector>
#include "../vector.h"
#include "test.h"

namespace mjstl
{
namespace test
{
namespace vector_test
{

void print_vector(mjstl::vector<int> v)
{
    for(auto it = v.begin(); it != v.end(); ++it)
        std::cout<<" "<<*it;
    std::cout<<std::endl;
}

void vector_test()
{
    std::cout<<"[===============================================================]"<<std::endl;
    std::cout<<"[---------------- Run container test : vector ------------------]"<<std::endl;
    std::cout<<"[---------------------------API test----------------------------]"<<std::endl;

    int a[] = {1,2,3,4,5,6,7,8,9,10};

    mjstl::vector<int> v;
    mjstl::vector<int> v1(a,a+(sizeof(a)/sizeof(int)));
    mjstl::vector<int> v2{10,100,1000,10000,100000};
    mjstl::vector<int> v3 = {1,11,111,1111,11111,111111};
    mjstl::vector<int> v4(v1);
    mjstl::vector<int> v5(std::move(v2));
    mjstl::vector<int> v6 = v3;
    mjstl::vector<int> v7 = std::move(v3);

    FUN_AFTER(v1,v1.push_back(1));
    FUN_AFTER(v1,v1.push_back(2));
    FUN_AFTER(v1,v1.push_back(3));
    FUN_AFTER(v1,v1.push_back(4));
    FUN_AFTER(v1,v1.emplace_back(4));
    FUN_AFTER(v1,v1.emplace_back(4));
    FUN_AFTER(v1,v1.pop_back());
    FUN_AFTER(v1,v1.pop_back());

    FUN_VALUE(*(v1.end()-1));
    FUN_VALUE(*(v1.rend()-1));
    FUN_VALUE(*v1.begin());
    FUN_VALUE(*v1.rbegin());
    FUN_VALUE(v1.front());
    FUN_VALUE(v1.back());
    FUN_VALUE(v1.at(v1.size()-1));
    FUN_VALUE(v1[0]);

    FUN_AFTER(v1,v1.assign(10,100));
    FUN_VALUE(v1.size());
    FUN_AFTER(v1,v1.assign(20));
    FUN_VALUE(v1.size());

    FUN_AFTER(v1,v1.insert(v1.begin()));
    FUN_AFTER(v1,v1.insert(v1.end()));
    FUN_AFTER(v1,v1.insert(v1.begin() + v1.size()/2,10000));
    FUN_AFTER(v1,v1.insert(v1.begin() + 1,10,111));
    FUN_AFTER(v1,v1.assign(a,a + (sizeof(a)/sizeof(int))));
    FUN_AFTER(v1,v1.insert(v1.begin() + 5,a,a + (sizeof(a)/sizeof(int))));
    FUN_AFTER(v1,v1.insert(v1.end() - 1,a,a + 3));
    FUN_AFTER(v1,v1.resize(v1.size() - 1));
    FUN_AFTER(v1,v1.resize(v1.size() + 10));
    FUN_AFTER(v1,v1.erase(v1.begin(),v1.end() - 1));
    FUN_AFTER(v1,v1.erase(v1.begin()));
    FUN_AFTER(v1,v1.swap(v4));
    FUN_AFTER(v1,v1.clear());
    FUN_VALUE(v1.size());
    
    std::cout<<std::boolalpha;
    FUN_VALUE(v1.size());
    FUN_AFTER(v1,v1.swap(v));
    FUN_VALUE(v1.empty());
    FUN_VALUE(v.size());
    FUN_AFTER(v1,v1.swap(v));
    FUN_VALUE(v1.empty());
    std::cout<<std::noboolalpha;
    PASSED;
#if PERFORMANCE_TEST_ON
    std::cout<<"[--------------------- Performance Testing ---------------------]"<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|      push_back      |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(vector<int>,push_back,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(vector<int>,push_back,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    PASSED;

// #if LARGER_TEST_DATA_ON
//     CON_TEST_P1(vector<int>,push_back,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
// #else
//     CON_TEST_P1(vector<int>,push_back,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
// #endif
//     std::cout<<std::endl;
//     std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
//     PASSED;
#endif
    std::cout<<"[---------------- End container test : vector ------------------]"<<std::endl;
}

} // namespace vector_test
} // namespace test
} // namespace mjstl
#endif//! __VECTOR_TEST_H__