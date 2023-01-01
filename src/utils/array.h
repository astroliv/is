#ifndef IS_ARRAY_H
#define IS_ARRAY_H

#include <cstdint>
#include "common.h"


//可变长数组
template<class T>
class Array {
    uint32_t capacity = 0;
    uint32_t usedSize = 0;
    T *data = nullptr;
public:
    Array();

    //以指定容量初始化
    explicit Array(uint32_t cap);

    //重置大小,会保留原数据,参数newCap为0则清空
    void resize(uint32_t newCap);

    //添加成员,不会扩容
    bool add(const T &value);

    //追加成员,自动扩容
    void append(const T &value);

    //弹出最后一个成员
    T &pop();

    //确保剩余空间足够达到指定大小,未达到则扩容
    void ensure(uint32_t remain);

    //设置指定idx处的成员内容.需保证idx<usedSize,超出范围则返回false
    void set(uint32_t idx, const T &value);

    //获取指定位置成员.需保证idx<usedSize,否则报错
    T &get(uint32_t idx);

    //返回数组最大容量
    uint32_t getCapacity();

    //返回数组已使用容量
    uint32_t getUsedSize();

    //获取指定位置成员.需保证idx<usedSize,否则报错
    T &operator[](uint32_t idx);
};


template<class T>
uint32_t Array<T>::getUsedSize() {
    return usedSize;
}

template<class T>
uint32_t Array<T>::getCapacity() {
    return capacity;
}

template<class T>
Array<T>::Array() {
    NULL;//为了防止CLion的Warning,写了个废话
}

template<class T>
T &Array<T>::pop() {
    assert(usedSize != 0, "No Array members can pop up.");
    return data[--usedSize];
}

template<class T>
T &Array<T>::operator[](uint32_t idx) {
    return get(idx);
}

template<class T>
T &Array<T>::get(uint32_t idx) {
    assert(idx < usedSize, "Index [%u] out of range.", idx);
    return data[idx];
}

template<class T>
void Array<T>::set(uint32_t idx, const T &value) {
    assert(idx < usedSize, "Index [%u] out of range.", idx);
    data[idx] = value;
}

template<class T>
void Array<T>::ensure(uint32_t remain) {
    if (capacity - usedSize >= remain) { return; }//判断剩余空间是否足够
    resize(ceilToPowerOf2(usedSize + remain));
    //不够就自动补上,顺便对齐二的次幂,可以减少扩容次数
}


template<class T>
Array<T>::Array(uint32_t cap) {
    resize(cap);
}

template<class T>
void Array<T>::append(const T &value) {
    if (capacity == usedSize) { resize(ceilToPowerOf2(capacity + 1)); }
    //不够就扩容,顺便对齐二的次幂,可以减少扩容次数
    data[usedSize++] = value;
}

template<class T>
bool Array<T>::add(const T &value) {
    if (capacity == usedSize) { return false; }
    data[usedSize++] = value;
    return true;
}

template<class T>
void Array<T>::resize(uint32_t newCap) { //这是variableArray的灵魂
    capacity = newCap;//先设置数量数据
    if (usedSize > newCap) { usedSize = newCap; }//将usedSize向下合理化
    if (newCap == 0) {
        if (data == nullptr) { return; }//无需清空的情况
        delete data;
        data = nullptr;
        return;
    }//此时为清空
    if (data == nullptr) {
        data = new T[newCap];
        return;
    }//如果为空则直接新建即可
    T *newPtr = new T[newCap];//创建新数组
    for (uint32_t i = 0; i < usedSize; ++i) { newPtr[i] = data[i]; }//复制内容
    delete data;  //删除旧数组
    data = newPtr;//使用新数组
}


#endif //IS_ARRAY_H
