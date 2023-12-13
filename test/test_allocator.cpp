#include "../allocator.h"

#include <vector>
#include <algorithm>
using namespace std;

int main(){
    int v[3] = {1,3,4};
    std::vector<int,ZMJ::allocator<int>> vec(v,v+3);
    for_each(vec.begin(),vec.end(),[](int i){ cout<<i<<" "; });
    return 0;
}