// 用115200波特率的串口RXD连接上PWM引脚，测试PWM控制器的计数频率是否为1GHz
// 测试结果：是的，PWM控制器的计数频率确实是1GHz。
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "PwmDef.hpp"
#include "KeyDef.hpp"
#include "test.hpp"

#define BAUDRATE 115200

int test_ensure_pwm_freq(void) {
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    std::shared_ptr<PwmDef> pwmPin = std::make_shared<PwmDef>(
        "pwmchip0", "0",
        (int)(1000000000/BAUDRATE*2), (int)(1000000000/BAUDRATE),
        "inversed", true );
    while(true) {
        if(kb->kbhit()) {
            char keyGet = kb->readKey();
            if(keyGet == KEY_ESC) break; // 按下ESC键退出
        }
        usleep(20);
    }
    return 0;
}