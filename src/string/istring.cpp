#include "istring.h"
#include <cstring>

isize cstrLengthof(const char *str) {
	isize length = 0;
	for (; str[length] != '\0'; ++length) {}
	return length;
}

//string元

inline isize stringMeta::getLength() const {
	return length;
}

inline const char *stringMeta::getData() {
	return data;
}

char *stringMeta::operator~() {
	return data;
}

char &stringMeta::operator[](isize idx) {
	assert(idx < length, "Index [%u] out of range.", idx)
	return data[idx];
}

bool stringMeta::operator==(const stringMeta &an) {
	if (length != an.length) { return false; }      //长度都不同,不可能相等
	if (data == an.data) { return true; }           //长度相同,且数据地址相同,为全等字符串
	for (isize i = 0; i < length; ++i) {            //遍历一下字符串的数据
		if (data[i] != an.data[i]) { return false; }//有不一样的地方就返回false
	}
	return true;
}

//引用式string

refString::refString(const char *str, isize len) {
	data = (char *) str;
	length = len;
}

refString::refString(const char *str) {
	data = (char *) str;
}

refString::refString(stringMeta &meta) {
	setMeta(meta);
}

inline void refString::setMeta(stringMeta &meta) {
	*(stringMeta *) this = meta;
}

inline void refString::setMeta(char *str, isize len) {
	data = str;
	length = len;
}

void refString::clearMeta() {
	length = 0;
	data = nullptr;
}

refString &refString::operator=(stringMeta &str) {
	data = (char *) str.getData();
	length = str.getLength();
	return *this;
}

refString &refString::operator=(const char *str) {
	data = (char *) str;
	for (; str[length] != '\0'; ++length) {}
	return *this;
}



//拷贝式string

string::string(const char *str, isize len) {
	init(str, len);
}

string::string(const char *str) {
	init(str);
}

string::string(stringMeta strMeta) {
	init(strMeta.getData(), strMeta.getLength());
}

string::string(const string &str) : stringMeta(str) {
	init(str.data, str.length);
}

string::~string() {
	clearMeta();
}

string &string::operator=(stringMeta &str) {
	clearMeta();
	init(str.getData(), str.getLength());
	return *this;
}

string &string::operator=(const char *str) {
	clearMeta();
	init(str);
	return *this;
}

inline void string::init(const char *str, isize len) {
	length = len;
	data = (char *) memcpy(new char[length + 1], str, length);
	data[length] = '\0';
}

void string::init(const char *str) {
	length = cstrLengthof(str);
	data = (char *) memcpy(new char[length + 1], str, length);
	data[length] = '\0';
}

void string::clearMeta() {
	length = 0;
	if (data == nullptr) { return; }
	delete[] data;
	data = nullptr;
}




