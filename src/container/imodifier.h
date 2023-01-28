//修饰器:为了将一组布尔信息作为bit存到一个integer内,节省空间,便写了这个东西
#ifndef IS_IMODIFIER_H
#define IS_IMODIFIER_H

#include "../utils/common.h"

//关于为什么叫Modifier(修饰器)而不叫Integrator(整合器):
//由于该工具是为了将函数与变量修饰符的有无信息整合储存起来,从而节省空间(开关信息用Array<bool>存过于浪费)
//即其维护的信息起到了修饰函数与变量的作用,于是叫它修饰器,即Modifier,要注意的是修饰符也是叫Modifier的,别搞混了(hh\)
//上面说的修饰信息(修饰符)表示诸如 对变量:isPtr,isMut,isStatic,isArray...; 对函数:isStatic,isExtern...
//这样的信息.  话说这可能是我注释写得最密的一个源文件了hh

//E即enum类型,I即对应的用于存储的integer类型
template<typename E, typename I>
class Modifier {
public:
	I data{0};                          //该类仅维护一个I类型的数值

	void set(E enumValue, bool value);  //设置该enum对应bit的值为value
	bool get(E enumValue);              //返回该enum对应bit的值

	bool operator[](E enumValue);       //返回该enum对应bit是否为1
	bool operator[](uint8_t idx);       //返回该idx对应的bit是否为1,idx需大于0
};

template<typename E, typename I>
inline void Modifier<E, I>::set(E enumValue, bool value) {
	auto idx = (uint8_t) enumValue;             //idx的值即表达要设置的bit,idx为2则设置第二个bit
	assert(idx > 0, "Unsupported behaviour!");  //eValue对应值不应小于1,因为不存在第0个bit或以下
	if (value) { data |= 1 << (idx - 1); }      //设置值要分value是true或false的情况
	else { data &= ~(1 << (idx - 1)); }
	//下面是举例注解
	//true  -> or
	//   10011101    |     10011001
	// | 00000100    |   | 00000100
	// ------------  |   ------------
	//   10011101    |     10011101

	//false -> not + and
	//   10011101    |      10011001
	// & 11111011    |    & 11111011
	// ------------  |     ------------
	//   10011001    |      10011001
}

template<typename E, typename I>
inline bool Modifier<E, I>::get(E enumValue) {
	assert((uint8_t) enumValue > 0, "Unsupported behaviour!");
	return data & (1 << ((uint8_t) enumValue - 1));
	//下面是举例注解
	//   10011101    |      10011001
	// & 00000100    |    & 00000100
	// ------------  |    ------------
	//   00000100    |      00000000
	// != 0 is true  |    == 0 is false
}

template<typename E, typename I>
bool Modifier<E, I>::operator[](E enumValue) {
	return get(enumValue);
}

template<typename E, typename I>
bool Modifier<E, I>::operator[](uint8_t idx) {
	assert((uint8_t) idx > 0, "Unsupported behaviour!");
	return data & (1 << (idx - 1));
}

#endif //IS_IMODIFIER_H
