// 测试串口
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "UartDef.hpp"
#include "test.hpp"

int test_uart_txrx(void) {
    std::shared_ptr<LS_UART> uart1 = std::make_shared<LS_UART>(UART1, B115200, LS_UART_STOP1, LS_UART_DATA8, LS_UART_NONE);
    char str[256];
    cerr << "正在接收中......" << endl;
    while(true) {
        int ret = uart1->ReadData(str, 256); //读取串口缓存区，最大读取256个字节
        if(ret < 0) {
            cerr << "读取的时候出现错误！" << endl;
            return 0;
        }
        else if(ret == 0) {
            continue;
        }
        else {
            str[ret] = '\0';
            printf("接收到了 %d 个字节，数据为：\n%s\n", ret, str);
            ret = uart1->WriteData(str, ret);
            if(ret < 0) {
                cerr << "回传的时候出现错误！" << endl;
                return 0;
            }
        }
    }
    return 0;
}