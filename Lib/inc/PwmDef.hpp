#ifndef __PWMDEF_HPP
#define __PWMDEF_HPP
#include "FileOption.hpp"
#include <string>       // 提供 std::string 类，用于处理字符串，方便进行字符串操作，如拼接、查找等。
#include <fcntl.h>      // 定义了文件控制相关的常量、类型和函数，例如 open 函数用于打开文件，可设置文件打开模式等。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include <stdexcept>    // 定义标准异常类，用于抛出和捕获运行时异常，便于错误处理。
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <fstream>      // 用于文件流操作，可进行文件的读写。
#include <sstream>      // 提供字符串流功能，可在字符串和其他数据类型间转换。
#include <cstring>      // 包含许多操作C风格字符串（字符数组）的函数，如 strcpy 、 strlen 等。
#include <memory>       // 智能指针

#define PWM0 "pwmchip0" // GPIO64
#define PWM1 "pwmchip1" // GPIO65
#define PWM2 "pwmchip2" // GPIO66
#define PWM3 "pwmchip3" // GPIO67

class PwmDef: public FileOption {
private:
    int     _fd; // 文件描述符
    string  _Mold; // pwmchip几
    string  _Channel; // 第几个通道
    int     _Period; // 周期计数，计数频率为1GHz
    int     _Duty_Cycle; // 占空计数
    string  _Polarity; // 指定高低电平占空
    bool    _is_enable; // 使能
public:
    PwmDef(const string &mold, const string& channel, int period, int duty_cycle, const string &polarity, bool is_enable);
    int pwmExport(void);
    int pwmUnexport(void);
    int setPeriod(int period);
    int setDuty(int duty_cycle);
    int setPolarity(const string &polarity);
    int setCmd(bool is_enable);
    ~PwmDef();
};

#endif /* #ifndef __PWM_LED_BREATH_HPP */