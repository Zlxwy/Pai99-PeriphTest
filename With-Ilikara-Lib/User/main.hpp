#ifndef __MAIN_HPP
#define __MAIN_HPP

#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <stdlib.h>     // 提供标准库函数，如内存分配、程序控制等。
#include <stdint.h>     // 提供固定宽度的整数类型，如 uint8_t 、 int32_t 等。
#include <termios.h>    // 提供终端控制相关的函数，例如设置终端属性等。
#include <string>       // 提供 std::string 类，用于处理字符串，方便进行字符串操作，如拼接、查找等。
#include <fcntl.h>      // 定义了文件控制相关的常量、类型和函数，例如 open 函数用于打开文件，可设置文件打开模式等。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include <stdexcept>    // 定义标准异常类，用于抛出和捕获运行时异常，便于错误处理。
#include <fstream>      // 用于文件流操作，可进行文件的读写。
#include <sstream>      // 提供字符串流功能，可在字符串和其他数据类型间转换。
#include <cstring>      // 包含许多操作C风格字符串（字符数组）的函数，如 strcpy 、 strlen 等。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <vector>       // 提供动态数组功能，如 vector ，用于存储和管理数据集合。
#include <array>        // 提供固定大小的数组功能，如 array 。
#include <thread>       // 提供线程创建、管理等功能。
#include <chrono>       // 提供时间相关的功能，如计时器。
#include <atomic>       // 提供原子操作，用于多线程编程中的数据安全访问。
#include <opencv2/opencv.hpp>

#endif /* #define __MAIN_HPP */