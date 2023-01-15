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
};

//引用式string

refString::refString(const char *str, isize len) {
	data = (char *) str;
	length = len;
}

refString::refString(const char *str) {
	setData((char *) str);
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

inline void refString::setData(char *str, bool checkLen) {
	data = (char *) str;
	if (checkLen) {
		length = 0;
		for (; str[length] != '\0'; ++length) {}
	}
}

void refString::setLength(isize len) {
	length = len;
}

void refString::clearMeta() {
	length = 0;
	data = nullptr;
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

string &string::operator=(const string &str) {
	clearMeta();
	init(str.data, str.length);
	return *this;
}

string &string::operator=(const char *str) {
	clearMeta();
	init(str);
	return *this;
}

void string::init(const char *str, isize len) {
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



