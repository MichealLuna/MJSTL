#ifndef __LIST_TEST_H__
#define __LIST_TEST_H__

#include "test.h"
#include "../list.h"
#include <list>

namespace mjstl
{
namespace test
{
namespace list_test
{
    
void list_test()
{
    std::cout<<"[===============================================================]"<<std::endl;
    std::cout<<"[----------------- Run container test : list -------------------]"<<std::endl;
    std::cout<<"[---------------------------API test----------------------------]"<<std::endl;
    
    int a[] = {1,2,3,4,5,6,7,8,9,10};
    COUT(a);
    mjstl::list<int> l1;
    COUT(l1);
    mjstl::list<int> l2(16);
    COUT(l2);
    mjstl::list<int> l3(16,100);
    COUT(l3);
    mjstl::list<int> l4{1,2,3,4,5,6,7,8,9,10};
    COUT(l4);
    mjstl::list<int> l5(a,a+sizeof(a)/sizeof(int));
    COUT(l5);
    mjstl::list<int> l6(l2);
    COUT(l6);
    mjstl::list<int> l7(std::move(l2));
    COUT(l7);
    mjstl::list<int> l8 = l3;
    COUT(l8);
    mjstl::list<int> l9 = std::move(l3);
    COUT(l9);
    mjstl::list<int> l10 = {1,2,3,4,5,6,7,8,9,0};
    COUT(l10);

    std::cout<<std::boolalpha;
    FUN_AFTER(l1,l1.assign(10));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.assign(10,100));
    FUN_AFTER(l1,l1.assign(a,a + sizeof(a)/sizeof(int)));
    FUN_AFTER(l1,l1.insert(l1.begin(),101));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.insert(l1.begin()));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.insert(l1.begin(),5,102));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.insert(l1.begin(),a,a + sizeof(a)/sizeof(int)));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.erase(l1.begin()));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.erase(l1.begin(),++l1.begin()));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.clear());
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.push_front(1));
    FUN_VALUE(l1.size());
    FUN_VALUE(l1.front());
    FUN_AFTER(l1,l1.push_front(10));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.push_back(10));
    FUN_VALUE(l1.size());
    FUN_VALUE(l1.back());
    FUN_AFTER(l1,l1.push_back(100));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.pop_back());
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.resize(10,108));
    FUN_VALUE(l1.size());

    FUN_VALUE(l10.size());
    FUN_AFTER(l1,l1.swap(l10));
    COUT(l10);
    FUN_VALUE(l1.size());
    FUN_VALUE(l10.size());

    //FUN_AFTER(l1,l1.sort());
    FUN_AFTER(l1,l1.merge(l10));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1,l1.sort());

    FUN_AFTER(l1,l1.unique());
    FUN_AFTER(l1,l1.remove(108));
    FUN_AFTER(l1,l1.remove_if([](int x){ return x == 0;}));
    FUN_AFTER(l1,l1.reverse());
    PASSED;

#if PERFORMANCE_TEST_ON
    std::cout<<"[--------------------- Performance Testing ---------------------]"<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    std::cout<<"|     push_back       |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(list<int>,push_back,rand(),SCALE_LL(LEN1),SCALE_LL(LEN2),SCALE_LL(LEN3));
#else
    CON_TEST_P1(list<int>,push_back,rand(),SCALE_L(LEN1),SCALE_L(LEN2),SCALE_L(LEN3));
#endif
    std::cout<<std::endl;
    std::cout<<"|---------------------|-------------|-------------|-------------|"<<std::endl;
    PASSED;
#endif
    std::cout<<"[------------------ End container test : list ------------------]"<<std::endl;

    std::cout<<std::noboolalpha;
}

/*
*
*   debug:
* 让我目前都不太明白的bug。内存崩溃。
*   1、resize()调用栈发现allocate内存分配失败。
*   2、因此怀疑内存分配器可能有问题。
*   3、但是内存分配器使用了很多测试，都没问题。
*   4、顺着递归调用往上，发现__fill_insert,也没有问题。
*   5、在往上insert，resize也实在是发现不了问题。
*   6、这时发现打印链表什么也没有。
*   7、发现了迭代器begin，end总是一样。
*   8、查找begin，end的返回迭代器是不是错了，没错。
*   9、再找迭代器这个类是不是有问题，没问题。
*   10、再找发现初始化initialize也没问题。
*   11、再看看insert（），这时候再次回到insert，
*      一行一行看，最后终于发现其中两行：
    * 调试半天不对，发现这两行有问题。
    *   position.node->prev = tmp;
    *   position.node->prev->next = tmp;
    * 
    *   这里小小的顺序，出大问题,如果先position.node->prev被覆盖为tmp，
    *  position.node->prev就成了tmp,而不是原来的position->prev。
    * 
    * 也可以这样避免：
    * link_type node_prev = position.node->prev;
    * position.node->prev = tmp;
    * node_prev->next     = tmp;
    * 果然使用临时变量也是有减少犯错、并容易发现bug的。
    * 
    * 刚开始的方法就不容易发现bug。总之链表特别注意next，prev初始化先后顺序。
    *
    * 
  最后：bug时修好了，但是我又迷惑了，这个只是连接顺序不对，为什么
  会导致申请内存崩溃？
    原因：在resize之前还调用过erase，由于前面insert错误的结点初始化
  顺序，所以erase把头结点node_误删除了。那么好了，把node结点放回了
  内存池中，但是又一直修改头节点的next,prev字段进行insert等操作，
  这样就改变了放回内存池中内存块的内容，那里其实是一个指向下一个空
  闲块的内存，被变成莫名奇妙的野指针，一进行解引用就导致了违规访问。

* 这次改bug再次让我震惊，因为看起来毫不相干的两个地方，居然关联了起来，
*而且仅仅是两行代码顺序反了。
*
*
*/

} // namespace list_test
} // namespace test
} // namespace mjstl
#endif// !__LIST_TEST_H__