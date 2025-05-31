#ifndef __GPIODEF_HPP
#define __GPIODEF_HPP
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
using namespace std;

#define  MODE_IN    "in"
#define  MODE_OUT   "out"

class GpioDef: public FileOption {
public:
    GpioDef();
    GpioDef(int gpio, const string &dir);

    int GpioExport();
    int GpioUnexport();
    int setDir(const string &dir);
    int setVal(int val);
    int getVal();
    
    ~GpioDef();
public:
    int _fd; // 文件描述符
    int _Gpio; //gpio引脚
    string _Dir; // GPIO输入输出
    int _Val;
};

#endif // #ifndef __GPIO_HPP