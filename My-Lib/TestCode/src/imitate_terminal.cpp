// 模仿终端输入
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <string>       // 提供 std::string 类，用于处理字符串，方便进行字符串操作，如拼接、查找等。
#include <sstream>      // 提供字符串流功能，可在字符串和其他数据类型间转换。
#include <cstring>      // 包含许多操作C风格字符串（字符数组）的函数，如 strcpy 、 strlen 等。
#include <cstdlib>
#include "test.hpp"
// &'Droid Sans Mono', 'monospace', monospace
int test_imitate_terminal(void) {
    std::string command;
    while (true) {
        std::cout << "$ "; // 显示提示符
        std::getline(std::cin, command); // 获取用户输入的命令

        if (command == "exit") break; // 如果用户输入exit，则退出程序
        int result = system(command.c_str()); // 执行系统命令
        if (result != 0) std::cerr << "Error executing command: " << command << std::endl;
    }
    return 0;
}