//不定长数组的实现,是本程序的基础容器
#ifndef IS_IARRAY_H
#define IS_IARRAY_H

#include <cstdint>
#include "../utils/common.h"

template<class T>
using EqFn = bool (*)(const T &, const T &);   //自定义比较类型T是否相等的函数,为了解决操作符==的逻辑无法满足要求的情况

//可变长数组
template<class T>
class Array {
protected:
	isize capacity = 0;
	isize usedSize = 0;
	T *data = nullptr;
public:
	Array() = default;                  //无参构造
	explicit Array(isize cap);          //容量构造

	~Array();                           //析构函数

	void resize(isize newCap);          //重置大小,会保留原数据,参数newCap为0则清空
	void ensure(isize remain);          //确保剩余空间足够达到指定大小,未达到则扩容

	bool add(const T &value);           //添加成员,不会扩容
	bool add();                         //添加空成员,不会扩容
	void append(const T &value);        //追加成员,自动扩容
	void append();                      //追加空成员,自动扩容
	T &pop();//弹出最后一个成员

	void set(isize idx, const T &value);//设置指定idx处的成员内容.需保证idx<usedSize,超出范围则返回false
	T &get(isize idx);                  //获取指定位置成员.需保证idx<usedSize,否则报错
	T *getPtr(isize idx);               //获取指定位置成员的指针,这是个不太安全的方法,但是它是需要的

	isize find(const T &value);         //正序寻找指定的值,并返回其在改容器内的idx,找不到就返回usedSize
	isize bfind(const T &value);        //倒序寻找指定的值,并返回其在改容器内的idx,找不到就返回usedSize
	isize reg(const T &value);          //注册value,若数组內无该内容则append并返回其idx,否则返回有相同内容的成员的idx
	isize breg(const T &value);         //注册value,若数组內无该内容则append并返回其idx,否则返回有相同内容的成员的idx,该函数的查找使用倒序的

	isize find(const T &value, EqFn<T> fn);         //此方法将使用fn进行比较
	isize bfind(const T &value, EqFn<T> fn);        //此方法将使用fn进行比较
	isize reg(const T &value, EqFn<T> fn);          //此方法将使用fn进行比较
	isize breg(const T &value, EqFn<T> fn);         //此方法将使用fn进行比较

	void clear();                       //清除数据

	isize getCapacity();                //返回数组最大容量
	isize getUsedSize();                //返回数组已使用容量

	void setout(Array<T> &ary);         //将当前数组数据迁出到ary
	isize setout(T *&ptr);              //将当前数组数据迁出
	T *setout();                        //将当前数组数据迁出

	T &operator[](isize idx);           //获取指定位置成员.需保证idx<usedSize,否则报错
};


template<class T>
Array<T>::Array(isize cap) {
	resize(cap);
}

template<class T>
Array<T>::~Array() {
	resize(0);
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
	T *newPtr = new T[newCap];          //创建新数组
	isize loop = MIN(usedSize, newCap); //因为有可能newCap<usedSize
	for (isize i = 0; i < loop; ++i) { newPtr[i] = data[i]; }//复制内容
	delete[] data;  //删除旧数组
	data = newPtr;  //使用新数组
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
bool Array<T>::add() {
	T value{};
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
void Array<T>::append() {
	T value{};
	if (capacity == usedSize) { resize(ceilToPowerOf2(capacity + 1)); }
	//不够就扩容,顺便对齐二的次幂,可以减少扩容次数
	data[usedSize++] = value;
}

template<class T>
T &Array<T>::pop() {
	assert(usedSize != 0, "No Array members can read su.");
	return data[--usedSize];
}

template<class T>
void Array<T>::set(isize idx, const T &value) {
	assert(idx < usedSize, "Index:%u out of range.", idx);
	data[idx] = value;
}

template<class T>
T &Array<T>::get(isize idx) {
	assert(idx < usedSize, "Index:%u out of range.", idx);
	return data[idx];
}

template<class T>
inline T *Array<T>::getPtr(isize idx) {
	assert(idx < usedSize, "Index:%u out of range.", idx);
	return data + idx;
}

template<class T>
isize Array<T>::find(const T &value) {
	for (isize i = 0; i < usedSize; ++i) {
		if (data[i] == value) { return i; }
	}
	return usedSize;
}

template<class T>
isize Array<T>::bfind(const T &value) {
	for (isize i = usedSize - 1; i != -1; --i) {
		if (data[i] == value) { return i; }
	}
	return usedSize;
}

template<class T>
isize Array<T>::reg(const T &value) {
	isize idx = find(value);
	if (idx == usedSize) { append(value); }
	return idx;
}

template<class T>
isize Array<T>::breg(const T &value) {
	isize idx = bfind(value);
	if (idx == usedSize) { append(value); }
	return idx;
}

template<class T>
isize Array<T>::find(const T &value, EqFn<T> fn) {
	for (isize i = 0; i < usedSize; ++i) {
		if (fn(data[i], value)) { return i; }
	}
	return usedSize;
}

template<class T>
isize Array<T>::bfind(const T &value, EqFn<T> fn) {
	for (isize i = usedSize - 1; i != -1; --i) {
		if (fn(data[i], value)) { return i; }
	}
	return usedSize;
}

template<class T>
isize Array<T>::reg(const T &value, EqFn<T> fn) {
	isize idx = find(value, fn);
	if (idx == usedSize) { append(value); }
	return idx;
}

template<class T>
isize Array<T>::breg(const T &value, EqFn<T> fn) {
	isize idx = bfind(value, fn);
	if (idx == usedSize) { append(value); }
	return idx;
}

template<class T>
inline void Array<T>::clear() {
	resize(0);
}

template<class T>
inline isize Array<T>::getCapacity() {
	return capacity;
}

template<class T>
inline isize Array<T>::getUsedSize() {
	return usedSize;
}

template<class T>
inline void Array<T>::setout(Array<T> &ary) {
	ary.data = data, ary.usedSize = usedSize, ary.capacity = capacity;
	data = nullptr, capacity = usedSize = 0;
}

template<class T>
inline isize Array<T>::setout(T *&ptr) {
	isize ret = usedSize;//仅返回有效数据大小
	ptr = data, data = nullptr, capacity = usedSize = 0;
	return ret;
}

template<class T>
inline T *Array<T>::setout() {
	T *_data = data;
	data = nullptr, capacity = usedSize = 0;
	return _data;
}

template<class T>
inline T &Array<T>::operator[](isize idx) {
	return get(idx);
}

//字节流
using ByteStream = Array<byte>;


#endif //IS_IARRAY_H
