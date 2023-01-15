//不定长数组的实现,是本程序的基础容器
#ifndef IS_IARRAY_H
#define IS_IARRAY_H

#include <cstdint>
#include "common.h"


//可变长数组
template<class T>
class Array {
	isize capacity = 0;
	isize usedSize = 0;
	T *data = nullptr;
public:
	Array() = default;                  //无参构造
	explicit Array(isize cap);          //容量构造

	void resize(isize newCap);          //重置大小,会保留原数据,参数newCap为0则清空
	void ensure(isize remain);          //确保剩余空间足够达到指定大小,未达到则扩容

	bool add(const T &value);           //添加成员,不会扩容
	void append(const T &value);        //追加成员,自动扩容
	T &pop();//弹出最后一个成员

	void set(isize idx, const T &value);//设置指定idx处的成员内容.需保证idx<usedSize,超出范围则返回false
	T &get(isize idx);                  //获取指定位置成员.需保证idx<usedSize,否则报错

	isize getCapacity();                //返回数组最大容量
	isize getUsedSize();                //返回数组已使用容量

	T &operator[](isize idx);           //获取指定位置成员.需保证idx<usedSize,否则报错
};

template<class T>
Array<T>::Array(isize cap) {
	resize(cap);
}

template<class T>
void Array<T>::resize(isize newCap) { //这是variableArray的灵魂
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
	for (isize i = 0; i < usedSize; ++i) { newPtr[i] = data[i]; }//复制内容
	delete data;  //删除旧数组
	data = newPtr;//使用新数组
}

template<class T>
void Array<T>::ensure(isize remain) {
	if (capacity - usedSize >= remain) { return; }//判断剩余空间是否足够
	resize(ceilToPowerOf2(usedSize + remain));
	//不够就自动补上,顺便对齐二的次幂,可以减少扩容次数
}

template<class T>
bool Array<T>::add(const T &value) {
	if (capacity == usedSize) { return false; }
	data[usedSize++] = value;
	return true;
}

template<class T>
void Array<T>::append(const T &value) {
	if (capacity == usedSize) { resize(ceilToPowerOf2(capacity + 1)); }
	//不够就扩容,顺便对齐二的次幂,可以减少扩容次数
	data[usedSize++] = value;
}

template<class T>
T &Array<T>::pop() {
	assert(usedSize != 0, "No Array members can pop up.");
	return data[--usedSize];
}

template<class T>
void Array<T>::set(isize idx, const T &value) {
	assert(idx < usedSize, "Index [%u] out of range.", idx);
	data[idx] = value;
}

template<class T>
T &Array<T>::get(isize idx) {
	assert(idx < usedSize, "Index [%u] out of range.", idx);
	return data[idx];
}

template<class T>
isize Array<T>::getCapacity() {
	return capacity;
}

template<class T>
isize Array<T>::getUsedSize() {
	return usedSize;
}

template<class T>
T &Array<T>::operator[](isize idx) {
	return get(idx);
}

//指令流
typedef Array<byte> Instream;


#endif //IS_IARRAY_H
