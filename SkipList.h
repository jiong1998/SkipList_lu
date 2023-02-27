//
// Created by 13249091467 on 2023/2/25.
//

#ifndef SKIPLIST_LU_SKIPLIST_H
#define SKIPLIST_LU_SKIPLIST_H
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define STORE_FILE "store/dumpFile"

std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

template<typename K, typename V>
class Node{
public:

    Node(K k, V v, int level);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V v);

    int node_level;//当前节点的层数

    Node<K,V> ** forward;//每个节点的forward成员函数是一个指向Node的指针数组，作用：Node_A->forward[x]保存的是当前节点A的第x层的下一个Node指针。

private:
    K Key;
    V Value;

};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level)
{
    //1. 给出Key and Value， level。
    //2. 根据level，初始化forward指针数组。

    this->Key = k;
    this->Value = v;
    this->node_level = level;

    this->forward = new Node<K,V> * [level+1];//创建一个保存Node*的指针数组，长度为level+1
    memset(this->forward, 0, sizeof(Node<K,V>*) * (level+1));
}

template<typename K, typename V>
Node<K,V>::~Node()
{
    delete [] forward;
}

template<typename K, typename V>
K Node<K, V>::get_key() const
{
    return this->Key;
}

template<typename K, typename V>
V Node<K,V>::get_value() const
{
    return this->Value;
}

template<typename K, typename V>
void Node<K, V>::set_value(V v)
{
    this->Value = v;
}

//--------------------SKIPLIST--------------------------------------------------------------------------------------------

template<typename K, typename V>
class SkipList
{
public:
    // 构造和析构函数，创建 Node 节点函数
    SkipList(int max_level);
    ~SkipList();
    Node<K, V> * create_node(K k, V v, int level);

    //增删查操作函数
    int insert_element(K, V);// 增
    int delete_element(K); // 删
    bool search_element(K);// 查Key对应的Value

    void display_list(); //打印跳表
    void clear();   //清空跳表
    int size(); //返回跳表节点数（不包含头节点）
    int level_length(); //返回层高

    //数据落盘和加载
    void dump_file();
    void load_file();

    //随机获得层高函数
    int get_random_level();

private:
    // 数据加载相关函数, 用来区分 key 和 value
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:
    int _max_level;  	   // 跳表层数上限
    int _skip_list_level;  // 当前跳表的最高层
    int _element_count;    // 跳表中节点数
    Node<K, V> * _header;   // 跳表中头节点指针

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;
};

template<typename K, typename V>
SkipList<K,V>::SkipList(int max_level)
{
    //初始化跳表
    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    K k;
    V v;
    // create header node and initialize key and value to null
    this->_header = new Node<K, V>(k, v, _max_level);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList<K, V>()
{
    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(K k, V v, int level)
{
    Node<K, V> * new_node = new Node<K, V>(k, v, level);
    return new_node;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level()
{
    int level = 1;

    //随机算法
    while(rand() % 2)
    {
        level++;
    }

    return (level<_max_level)? level: _max_level;// 最大不超过上限
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(K k, V v)
{
    mtx.lock();// 写操作，加锁

    //用current指针遍历节点
    Node<K, V> * current = this->_header;// 从头节点遍历


    // update 是一个指针数组，数组内存放指针，指向 node 节点，其索引代表的是层数。
    // update[x]保存的是第x层最后一个符合要求的节点（要求：只要当前节点的下一节点非空，且 key 小于目标）
    Node<K, V> * update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V> *) * (_max_level+1));

    //从顶层开始寻找每一层符合条件的最后一个节点，保存到update数组中
    for(int i = _skip_list_level; i>=0; --i)
    {
        // 只要当前节点的下一节点非空，且 key 小于目标, 就会向后遍历
        while(current->forward[i] != NULL && current->forward[i]->get_key()<k)//如果符合条件
        {
            // 节点向后移动，这里你就把forward[i]理解为next就行
            current = current->forward[i];
        }
        update[i] = current;// update[i] 记录当前层最后符合要求的节点
    }

    // 遍历到 level 0 说明到达最底层了，forward[0]指向的就是跳表下一个邻近节点
    current = current->forward[0];//此时current所指的节点的key >= k

    // 1. 插入元素已经存在。插入失败！
    if(current != NULL && current->get_key() == k)
    {
//        std::cout << "key: " << k << ", exists" << std::endl;
        mtx.unlock();
        return -1;// 插入元素已经存在，返回 -1，插入失败
    }
    // 2. 如果当前 current 不存在，或者 current->get_key > key。则可以插入
    if(current == NULL || current->get_key() != k)
    {
        int level = get_random_level();

        // 如果新添加的节点层高level大于当前跳表层高，则需要更新 update 数组，
        // 将原本[_skip_list_level, random_level]范围内的NULL改为_header
        if(level > _skip_list_level)
        {
            for(int i = level; i>_skip_list_level; i--)
                update[i] = _header;

            _skip_list_level = level;// 最后更新跳表层高
        }

        //创建节点，开始插入
        Node<K, V> * new_node = create_node(k, v, level);

        //此时update保存了所有层数的符合条件的最后一个节点
        for(int i = 0; i<=level; i++)
        {
            // 该操作等价于:
            // new_node->next = pre_node->next;
            // pre_node->next = new_node; 只不过是逐层进行
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }
//        std::cout << "Successfully inserted key: " << k << ", value: " << v << std::endl;
        _element_count++;// 更新节点数
    }
    mtx.unlock();
    return 1;//插入成功
}

template<typename K, typename V>
int SkipList<K, V>::delete_element(K k)
{
    mtx.lock();

    Node<K, V> * current = _header;

    // 自顶向下找到要删除节点的前一个节点，并保存在指针数组update中。
    // 注：update[x]第x层保存要删除节点的前一个节点
    Node<K, V> * update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V> *) * (_max_level+1));

    for(int i = _skip_list_level; i>=0; --i)
    {
        while(current->forward[i]!=NULL && current->forward[i]->get_key() < k)
        {
            // 节点向后移动，这里你就把forward[i]理解为next就行
            current = current->forward[i];
        }
        //保存每一层的所删除节点的前一个节点
        update[i] = current;
    }

    current = current->forward[0];//此时，要不就指向所删除的节点，要不就说明该跳表中没有该节点
    if(current!=NULL && current->get_key() == k)
    {
        // 此时，current指向了所需要删除的节点

        // 从最底层开始删除,且update->forward 指向的节点为目标节点。
        for(int i=0; i<=_skip_list_level; ++i)
        {
            // 如果 update[i]->forward[i] 已经不指向 current,说明 i 的上层也不会指向 current
            // 也说明了被删除节点层高 i - 1。直接退出循环即可
            if(update[i]->forward[i] != current)
                break;

            // 删除操作，等价于 node->next = node->next->next
            update[i]->forward[i] = current->forward[i];
        }
        // 更新层高
        // 因为可能删除的元素它的层数恰好是当前跳跃表的最大层数
        // 所以此时需要重新确定 _skip_list_level,通过头节点判断
        while(_skip_list_level > 0 && _header->forward[_skip_list_level] == 0)
        {
            --_skip_list_level;
        }

        std::cout << "Successfully deleted key : "<< k << std::endl;
        --_element_count;
        delete current;
        mtx.unlock();
        return 1;// 返回值 1 说明成功删除

    }
    // 2. 笔者添加了没有该键时的情况，打印输出提示
    else
    {
        std::cout << k << " is not exist, please check your input !\n";
        mtx.unlock();
        return -1; // 返回值 -1 说明没有该键值
    }

}

template<typename K, typename V>
bool SkipList<K, V>::search_element(K k)
{
//    std::cout << "search_element..." << std::endl;

    //从顶向下查
    Node<K, V>* current = _header;

    // 从最高层开始遍历，找到最底层中最后一个满足小于key的节点
    for(int i = _skip_list_level; i >= 0; --i)
    {
        while(current->forward[i]!=NULL && current->forward[i]->get_key() < k) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];//此时current可能指向所查到的节点，也可能没有该节点

    //找到
    if(current!=NULL && current->get_key()==k)
    {
        std::cout << "Found key: " << k << ", value: " << current->get_value() << std::endl;
        return true;
    }
    // 没找到
//    else
//    {
//        std::cout << "Not Found Key: " << k << std::endl;
//        return false;
//    }

}

template<typename K, typename V>
void SkipList<K, V>::display_list()
{
    std::cout << "\n******** Skip List ********"<<"\n";
    //逐层打印
    Node<K, V> * current = _header;
    for(int i=0; i<=_skip_list_level; ++i)
    {
        Node<K, V> * node = _header->forward[i];
        std::cout << "Level " << i << ": ";
        while(node)
        {
            std::cout<<node->get_key()<<":"<<node->get_value()<<";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template<typename K, typename V>
int SkipList<K, V>::size()
{
    return _element_count;// 会随着节点的添加, 删除, 更新改变
}

template<typename K, typename V>
int SkipList<K, V>::level_length()
{
    return _skip_list_level;// 会随着节点的添加, 删除, 更新改变
}

template<typename K, typename V>
void SkipList<K, V>::clear()
{
    std::cout << "clear ..." << std::endl;
    Node<K, V> * current = _header->forward[0];
    while(current!=NULL)
    {
        Node<K, V> * temp = current;
        current = current->forward[0];
        delete temp;
    }
    // 重新初始化 _header
    for(int i = 0; i <= _skip_list_level; ++i)
    {
        _header->forward[i] = 0;
    }

    this->_element_count = 0;
    this->_skip_list_level = 0;
}

// 落盘、加载及其辅助函数
template<typename K, typename V>
void SkipList<K, V>::dump_file()
{
    std::cout << "dump_file..." << std::endl;
    _file_writer.open(STORE_FILE); // 打开文件，写操作
    Node<K, V> * node = this->_header->forward[0];
    // 只写入键值对，放弃层信息
    while(node != NULL)
    {
        // 文件写入（key value 以 : 为分隔符），及信息打印
        _file_writer << node->get_key() << ":" << node->get_value()<< "\n";
//        std::cout<<"已保存信息："<<node->get_key()<<":"<<node->get_value()<<";\n";
        node = node->forward[0];
    }
    _file_writer.flush(); //从缓冲区刷盘
    _file_writer.close();
    return ;
}

template<typename K, typename V>
void SkipList<K, V>::load_file()
{
    // 将数据从磁盘中导入内存，但是层数关系会发生变化
    std::cout << "load_file..." << std::endl;
    _file_reader.open(STORE_FILE);
    std::string line;

    // key 与 value 是一个指向 string 对象的指针
    std::string * key = new std::string();
    std::string * value = new std:: string();
    while(getline(_file_reader, line))// 一行一行写入
    {
        get_key_value_from_string(line, key, value);// 辅助函数
        if(key->empty() || value->empty())
            continue;
        // 重新载入过程使用 insert_element()
        // 所以层之间的关系(各节点的层高)可能发生变化, 所以与之前的SkipList不同
        try
        {
            //将*key由string转换为int
            int num = std::stoi(*key);
            insert_element(num, *value);
//            std::cout << "key:" << num << "value:" << *value << std::endl;
        }
        //如果字符串无法转换为整数，将会抛出 std::invalid_argument 异常或 std::out_of_range 异常
        catch (const std::invalid_argument& e)
        {
            std::cout << "Invalid argument: " << e.what() << std::endl;
        }
        catch (const std::out_of_range& e)
        {
            std::cout << "Out of range: " << e.what() << std::endl;
        }

    }
    _file_reader.close();
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string &str, std::string *key, std::string *value)
{
    // str的形式: 1:lu，这个函数负责将key和value提取出来保存到输出参数中
    if(!is_valid_string(str))
        return;

    // 分隔符之前的为 key, 分隔符之后的为 value
    *key = str.substr( 0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string &str)
{
    if(str.empty())
        return false;

    //没有发现分隔符
    if(str.find(delimiter) == std::string::npos)
        return false;
    return true;
}


#endif //SKIPLIST_LU_SKIPLIST_H