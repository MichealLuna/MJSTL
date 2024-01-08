#include "vector.h"
#include "algo.h"
#include <iostream>

using namespace mjstl;

int main(){
    vector<int> nums(10,2);
    vector<int> nums1(nums.begin(),nums.end());
    for_each(nums1.begin(),nums1.end(),[](int i){ std::cout<<i<<" ";});
}