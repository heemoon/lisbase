/*
 *文件名称：SH_Regex.h
 *功能描述：正则表达式
 *文件说明：
 *作 者：	李熙文
 *创建时间：2014-1-7
 *修改：
 */

//需要用到以下几个函数。（定义在/usr/include/regex.h文件中）
//int regcomp (regex_t *compiled, const char *pattern, int cflags)  
//int regexec (regex_t *compiled, char *string, size_t nmatch, regmatch_t matchptr [], int  eflags)
//void regfree (regex_t *compiled)
//size_t regerror (int errcode, regex_t *compiled, char *buffer, size_t length)
//1.int regcomp (regex_t *compiled, const char *pattern, int cflags)
//
//这个函数把指定的规则表达式pattern编译成一种特定的数据格式compiled，这样可以使匹配更有效。函数regexec 会使用这个数据在目标文
//本串中进行模式匹配。执行成功返回０。
//
//regex_t 是一个结构体数据类型，用来存放编译后的规则表达式，它的成员re_nsub 用来存储规则表达式中的子 规则表达式的个数，
//子规则表达式就是用圆括号包起来的部分表达式。
//
//pattern 是指向我们写好的规则表达式的指针。
//cflags 有如下4个值或者是它们或运算(|)后的值:
//REG_EXTENDED 以功能更加强大的扩展规则表达式的方式进行匹配。
//REG_ICASE 匹配字母时忽略大小写。
//REG_NOSUB 不用存储匹配后的结果。
//REG_NEWLINE 识别换行符，这样'$'就可以从行尾开始匹配，'^'就可以从行的开头开始匹配。
//
//2. int regexec (regex_t *compiled, char *string, size_t nmatch, regmatch_t matchptr [], int eflags)
//
//当我们编译好规则表达式后，就可以用regexec 匹配我们的目标文本串了，如果在编译规则表达式的时候没有指定cflags的参数为REG_NEWLINE，则默认情况下是忽略换行符的，也就是把整个文本串当作一个字符串处理。执行成功返回０。
//
//regmatch_t 是一个结构体数据类型，成员rm_so 存放匹配文本串在目标串中的开始位置，rm_eo 存放结束位
//置。通常我们以数组的形式定义一组这样的结构。因为往往我们的规则表达式中还包含子规则表达式。数组0单元存放主规则表达式位置，后边的单元依次存放子规则表达式位置。
//
//compiled 是已经用regcomp函数编译好的规则表达式。
//string 是目标文本串。
//nmatch 是regmatch_t结构体数组的长度。
//matchptr regmatch_t类型的结构体数组，存放匹配文本串的位置信息。
//eflags 有两个值
//REG_NOTBOL 按我的理解是如果指定了这个值，那么'^'就不会从我们的目标串开始匹配。总之我到现在还不是很明白这个参数的意义，
//原文如下：
//If this bit is set, then the beginning-of-line operator doesn't match the beginning of the
//string (presumably  because it's not the beginning of a line).If not set, then the beginning-of-line operator
//        does match the beginning  of the string.
//        REG_NOTEOL 和上边那个作用差不多，不过这个指定结束end of line。
//
//        3. void regfree (regex_t *compiled)
//
//        当我们使用完编译好的规则表达式后，或者要重新编译其他规则表达式的时候，我们可以用这个函数清空
//        compiled指向的regex_t结构体的内
//        容，请记住，如果是重新编译的话，一定要先清空regex_t结构体。
//
//        4. size_t regerror (int errcode, regex_t *compiled, char *buffer, size_t length)
//
//        当执行regcomp 或者regexec 产生错误的时候，就可以调用这个函数而返回一个包含错误信息的字符串。
//
//        errcode 是由regcomp 和 regexec 函数返回的错误代号。
//        compiled 是已经用regcomp函数编译好的规则表达式，这个值可以为NULL。
//        buffer 指向用来存放错误信息的字符串的内存空间。
//        length 指明buffer的长度，如果这个错误信息的长度大于这个值，则regerror 函数会自动截断超出的字符串，但他仍然会返回完整的字符串的长度。所以我们可以用如下的方法先得到错误字符串的长度。
//        size_t length = regerror (errcode, compiled, NULL, 0);
#ifndef __LISBASE_UTIL_REGEX_H__
#define __LISBASE_UTIL_REGEX_H__

#include <stdint.h>
#include <regex.h>
#include "CommonType.h"

namespace lisbase {

class CRegex {
public:
	CRegex();
	~CRegex();

public:
	BOOL init(const char* pattern, uint32_t nm = 10);
	void destroy();

	BOOL exec(char* pDest);

	int getFrom(uint32_t idx);
	int getLen(uint32_t idx);

	int64_t getIntByIdx(uint32_t idx, const char* pDest);
	BOOL writeStringByIdx(uint32_t idx, const char* pDest, char* pWrite);
private:
	regex_t m_Reg;
	regmatch_t m_aMatch[10];  //修改成栈操作
	uint32_t m_uNum;
};

}

#endif
