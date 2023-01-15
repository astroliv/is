//字符串类的实现 (注:有两种)
#ifndef IS_ISTRING_H
#define IS_ISTRING_H

#include "common.h"

isize cstrLengthof(const char *str);

//char *cstrCopy(const char *str, isize len);

//字符串数据元
class stringMeta {
protected:
	isize length{0};
	char *data{nullptr};

public:
	stringMeta() = default;         //无参构造

	isize getLength() const;        //获取字符串长度
	const char *getData();          //获取字符串数据指针

	char *operator~();              //获取字符串数据指针
	char &operator[](isize idx);    //取指定索引字符的引用

};

//引用式字符串
class refString : public stringMeta {
public:
	refString() = default;                  //无参构造
	refString(const char *str, isize len);  //补全构造
	explicit refString(const char *str);    //常量构造
	explicit refString(stringMeta &meta);   //基元构造

	void setMeta(stringMeta &meta);                  //设置基元
	void setMeta(char *str, isize len);              //设置基元
	void setData(char *str, bool checkLen = false);  //设置数据
	void setLength(isize len);                       //设置长度

	void clearMeta();                       //清空基元
};

//拷贝式字符串
class string : public stringMeta {
public:
	string() = default;                         //无参构造
	string(const char *str, isize len);         //补全构造
	explicit string(const char *str);           //常量构造
	explicit string(stringMeta strMeta);        //基元构造
	string(const string &str);                  //复制构造

	~string();                                  //析构

	string &operator=(const string &str);       //赋值重载
	string &operator=(const char *str);         //赋值重载

private:
	void init(const char *str, isize len);      //初始化
	void init(const char *str);                 //初始化

	void clearMeta();                           //清空基元
};


#endif //IS_ISTRING_H
