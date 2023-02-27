# Key-Value存储引擎
跳表：跳表（Skip List）是一种数据结构，它是一种基于有序链表的快速查找算法，类似于二叉查找树和平衡树等数据结构，但跳表的插入和删除操作的时间复杂度要比平衡树低，且实现简单。

跳表中的元素按照升序排列，并且每个元素有多个层次。每一层都是一个有序的链表，其中最底层的链表包含所有元素。上层链表是下层链表的子集，每个节点在每一层都有一个对应的节点。

在跳表中，查找元素时从顶层开始，逐层向下查找，直到找到目标元素或找到比目标元素大的元素位置，然后在当前层继续向下查找。这个查找过程就像是在一张跳板上跳跃，因此称为跳表。

跳表的时间复杂度为O(log n)，其中n为元素个数。对于插入、删除操作，跳表的时间复杂度也为O(log n)。跳表的实现可以使用链表或数组，因此跳表可以适应不同的应用场景。

非关系型数据库Redis以及Levedb、Rockdb等的核心存储引擎都采用了跳表数据结构。

![v2-46e6f807755e45ea1892912e9dcdc0d7_1440w](https://user-images.githubusercontent.com/77431730/221496447-00601b64-562a-402c-aa3b-9fb5907c7673.png)



本项目本着重复造轮子的心理（ps：才不是，只是为了锻炼下自己的基础），并结合程序员Carl的跳表实现（推荐大家去看看，链接放在最底下了），实现了一个轻量级键值型存储引擎，**采用C++实现，底层基于跳表实现。它支持数据的插入、删除、查询、展示、持久化、文件数据加载以及数据库大小的显示。**

# 提供的接口
- insert_element 插入数据
- delete_element 删除数据
- search_element 查询数据
- update_element 更新数据
- display_list 打印跳跃表
- dump_file 数据落盘
- load_file 加载数据
- size 返回数据规模
- level_length 返回层高
- clear 清空跳表

# 测试结果
## 插入测试

所插入数据的size（万）	10	50	100	1000
所消耗时间（秒）	0.130109	1.03466	2.29686	34.8959!

[image](https://user-images.githubusercontent.com/77431730/221499981-b29810f8-59c6-4cd1-b5cc-cf972d2aeec4.png)



最后，详细的项目讲解请进入我的博客观看：https://blog.csdn.net/weixin_45045804/article/details/129208028?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22129208028%22%2C%22source%22%3A%22weixin_45045804%22%7D

参考：https://github.com/youngyangyang04/Skiplist-CPP。 
