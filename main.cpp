#include <iostream>
#include "SkipList.h"
#include <string>
#include<ctime>
#include <cstdlib>
using namespace std;

// 生成指定长度的随机字符串
string generateRandomString() {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    string random_string = "";
    srand(time(NULL));
    for (int i = 0; i < rand() % 10; ++i) {
        random_string += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return random_string;
}

int main() {
    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    SkipList<int, std::string> skipList(7);


    // insert 测试
    for(int i=1; i<1000000; ++i)
        skipList.insert_element(i, generateRandomString());

    std::cout<<"节点数："<<skipList.size()<<endl;
    std::cout<<"层高:"<<skipList.level_length()<<endl;
    // display 测试
//    skipList.display_list();

    // delete 测试
    skipList.delete_element(1);
    skipList.delete_element(6);


    // search 测试
    skipList.search_element(3);
    skipList.search_element(6);

    // 落盘 测试
    clock_t startTime_dump, endTime_dump;//记时用
    startTime_dump = clock();//计时开始
    skipList.dump_file();
    endTime_dump = clock();//计时结束
    std::cout << "The 落盘 time is: " <<(double)(endTime_dump - startTime_dump) / CLOCKS_PER_SEC << "s" << std::endl;

    // 清空测试
    skipList.clear();

    // 加载 测试
    clock_t startTime_load,endTime_load;//记时用
    startTime_load = clock();//计时开始
    skipList.load_file();
    endTime_load = clock();//计时结束
    std::cout << "The 加载 time is: " <<(double)(endTime_load - startTime_load) / CLOCKS_PER_SEC << "s" << std::endl;

//    skipList.display_list();
}
