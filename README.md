# Key-Value存储引擎
跳表：增加了向前指针的链表叫作跳表。跳表全称叫做跳跃表，简称跳表。跳表是一个随机化的数据结构，实质就是一种可以进行二分查找的有序链表。跳表在原有的有序链表上面增加了多级索引，通过索引来实现快速查找。跳表不仅能提高搜索性能，同时也可以提高插入和删除操作的性能。（转自百度百科）非关系型数据库Redis以及Levedb、Rockdb等的核心存储引擎都采用了跳表数据结构。

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




最后，详细的项目讲解请进入我的博客观看：https://blog.csdn.net/weixin_45045804/article/details/129208028?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22129208028%22%2C%22source%22%3A%22weixin_45045804%22%7D

参考：https://github.com/youngyangyang04/Skiplist-CPP。 
