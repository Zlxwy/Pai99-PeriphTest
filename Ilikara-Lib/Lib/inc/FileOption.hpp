#ifndef __FILE_OPTION_HPP
#define __FILE_OPTION_HPP
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

class FileOption {
public:
    /*打开文件，打开失败返回-1,打开成功返回文件描述符*/
    /* 调用示例代码
     * int fd;
     * this->fd = this->openFile(path, O_WRONLY);
     * if(this->fd < 0) { //打开文件失败 } */
    int openFile(const string &file_path, int open_mode) { // 传入文件路径、打开模式
        int fd = open(file_path.c_str(), open_mode); // 获取文件描述符
        if(fd < 0) { // 如果打开文件不成功
            return -1; // 返回-1
        } else { // 如果文件打开成功
            return fd; // 返回文件描述符，以便后续文件操作
        }
    }
    /*写入文件，写入失败返回-1，写入成功返回写入数据的长度，最后都会关闭文件*/
    /* 调用示例代码
     * int write_data = 123456;
     * if(this->writeFile(this->fd, to_string(write_data)) < 0) { //写入文件失败 }*/
    int writeFile(int fd, const string &write_data) {
        int size = write_data.size(); // 计算数据长度
        int ret = 0; // 接收打开文件的返回数据
        ret = write(fd, write_data.c_str(), size); // 写入文件
        if(ret != size) { // 如果数据长度和写入长度不一致
            close(fd); // 关闭文件
            return -1; // 返回-1
        } else { // 如果数据长度和写入长度一致
            close(fd); // 关闭文件
            return ret; // 返回长度
        }
    }
    /*读取文件，读取失败则返回-1，读取成功则返回读取长度，最后都会关闭文件*/
    /* 调用示例代码
     * char *read_data = new char[2];
     * if(this->readFile(this->fd, read_data, 2) < 0) { //读取文件失败 }
     * delete[] read_data; */
    int readFile(int fd, char *read_data, ssize_t read_size) {
        int ret = read(fd, read_data, read_size);
        if(ret != read_size) {
            close(fd);
            return -1;
        } else {
            close(fd);
            return ret;
        }
    }
};

#endif