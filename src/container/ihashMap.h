//哈希表的实现,用于快速查找
#ifndef IS_IHASHMAP_H
#define IS_IHASHMAP_H

//键值对
template<typename K, typename V>
struct Entry {
	K key;
	V value;
};

//哈希表
template<typename K, typename V>
class HashMap {
	Entry<K, V> *data;  //数据存储区


};


#endif //IS_IHASHMAP_H
