//stack是一个基本容器,仅支持push&pop&[]操作
#ifndef IS_ISTACK_H
#define IS_ISTACK_H

#include "../utils/common.h"
#include "../report/ireport.h"

template<typename T>
class Stack {
public:
	T *bp{nullptr},         //栈基地址(指向栈底)
	*sp{nullptr},           //当前地址(指向第一个空的成员)
	*up{nullptr};           //栈顶地址(指向第一个无效成员)
	bool clean{true};       //是否可以释放bp,即是否拥有bp所有权

	Stack() = default;          //默认构造
	explicit Stack(isize cap);  //容量构造

	~Stack();                   //析构函数

	void resize(isize newCap);  //重置容器容量
	void ensure(isize remain);  //确保剩余成员数

	void push(const T &value);  //压入value栈
	const T &pop();             //弹出sp指向的成员

	void set(T *_bp, T *_sp, T *_up);       //设置数据
	void set(T *_bp, T *_up);               //设置数据
	void set(T *_bp, T *_up, isize us);     //设置数据
	void set(T *_bp, isize us, isize cap);  //设置数据
	void clear();                           //清除数据

	isize capacity();       //计算容量
	isize usedSize();       //计算已用容量

	T &operator[](isize idx); //获取指定索引的成员
};


template<typename T>
Stack<T>::~Stack() {
	resize(0);
}

template<typename T>
Stack<T>::Stack(isize cap) {
	resize(cap);
}

template<typename T>
void Stack<T>::resize(isize newCap) {
	if (newCap == 0) {//清空的情况
		if (clean && bp != nullptr) { delete[] bp; }
		bp = sp = up = nullptr;
	} else if (bp == nullptr) {//初始化的情况
		sp = bp = new T[newCap];
		up = bp + newCap;
	} else {//重设容量
		T *newBp = new T[newCap];
		isize valid = MIN(usedSize(), newCap);//重设容量后的有效成员数
		for (isize i = 0; i < valid; ++i) { newBp[i] = bp[i]; }//复制内容
		if (clean) { delete[] bp; }
		bp = newBp;
		sp = bp + valid;
		up = bp + newCap;
	}
	clean = true;//重设之后的指针将是新建的,所有权归该实例
}

template<typename T>
inline void Stack<T>::ensure(isize remain) {
	if (up - sp >= remain) { return; }
	resize(ceilToPowerOf2(usedSize() + remain));
}

template<typename T>
inline void Stack<T>::push(const T &value) {
	if (sp == up) {
		resize(ceilToPowerOf2(capacity() + 1));
		reportMsg(RepId::stackResized, nullptr, capacity());
	}
	*sp++ = value;
}

template<typename T>
inline const T &Stack<T>::pop() {
	assert(sp != bp, "The stack has no member!");
	return *--sp;
}

template<typename T>
inline void Stack<T>::set(T *_bp, T *_sp, T *_up) {
	assert(_bp <= _sp && _sp <= _up, "The required stack structure is unreasonable.");
	bp = _bp, sp = _sp, up = _up;
	clean = false;//外来数据,默认不能删除
}

template<typename T>
void Stack<T>::set(T *_bp, T *_up) {
	assert(_bp <= _up, "The required stack structure is unreasonable.");
	bp = sp = _bp, up = _up;
	clean = false;//外来数据,默认不能删除
}

template<typename T>
void Stack<T>::set(T *_bp, T *_up, isize us) {
	assert(_bp + us <= _up, "The required stack structure is unreasonable.");
	bp = _bp, sp = _bp + us, up = _up;
	clean = false;//外来数据,默认不能删除
}

template<typename T>
inline void Stack<T>::set(T *_bp, isize us, isize cap) {
	bp = _bp, sp = _bp + us, up = _bp + cap;
	clean = false;//外来数据,默认不能删除
}

template<typename T>
inline void Stack<T>::clear() {
	resize(0);
}

template<typename T>
inline isize Stack<T>::capacity() {
	return up - bp;
}

template<typename T>
inline isize Stack<T>::usedSize() {
	return sp - bp;
}

template<typename T>
inline T &Stack<T>::operator[](isize idx) {
	return bp[idx];
}

#endif //IS_ISTACK_H
