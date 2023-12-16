#include "../allocator.h"
#include "../sgi_allocator.h"

#include <string>
#include <list>
#include <queue>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <thread>
using namespace std;

enum{
    GKB = 1024,
    MKB = 1024*1024,
    GiB = 1024*1024*1024
};
struct BigBlock{
    int data[1024*1024*1024];
    void set(int val){
        for(int i=0;i<sizeof(data)/sizeof(int);i++)
            data[i] = val;
    }
};

int main(){
    //for vector
    int v[] = {1,3,4,13203,2302,23,34,3,53,23,23};
    std::vector<int,ZMJ::simple_alloc<int>> vec(v,v+sizeof(v)/sizeof(int));
    for_each(vec.begin(),vec.end(),[](int i){ cout<<i<<" "; });
    std::cout<<std::endl;

    //for map
    std::map<std::string,int,std::less<std::string>,ZMJ::simple_alloc<std::pair<const std::string,int>>> mp;
    std::string base = "HUTAO";
    mp.insert({"ZMJ",23});
    for(int i = 0; i < 1000; ++i){
        char buf[20] = {0};
        std::string tmp = base + itoa(i,buf,10);
        mp.insert({tmp,16});
    }
    for_each(mp.begin(),mp.end(),[](std::pair<std::string,int> i){ cout<<i.first<<" "<<i.second<<"\n"; });

    //for unordered_map : 这里还要给simple_alloc提供一个拷贝构造！simple_alloc(const simple_alloc<U>& other);
    std::unordered_map<std::string,int,hash<std::string>,equal_to<std::string>,ZMJ::simple_alloc<std::pair<const std::string,int>>> umap;
    for(int i = 0; i < 1000; ++i){
        char buf[20] = {0};
        std::string tmp = base + itoa(i,buf,10);
        umap.insert({tmp,16});
    }
    for_each(umap.begin(),umap.end(),[](std::pair<std::string,int> i){ cout<<i.first<<" "<<i.second<<"\n"; });

    //for list
    std::list<std::string,ZMJ::simple_alloc<std::string>> lt;
    for(int i = 0; i < 1000; ++i){
        lt.push_back("YOLA");
    }
    for_each(lt.begin(),lt.end(),[](std::string& i){ cout<<i<<"\n"; });

    //for dequeue
    std::deque<std::string,ZMJ::simple_alloc<std::string>> dt;
    for(int i = 0; i < 1000; ++i){
        dt.push_back("RaidenEi");
    }
    for_each(dt.begin(),dt.end(),[](std::string& i){ cout<<i<<"\n"; });

    //for big memory allocate.
    BigBlock* big = ZMJ::simple_alloc<BigBlock>().allocate(2);
    big->set(628);
    ZMJ::simple_alloc<BigBlock>().deallocate(big,2);
    return 0;
}