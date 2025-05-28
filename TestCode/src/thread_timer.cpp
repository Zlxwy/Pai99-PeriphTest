// 使用自己写的一个Timer类来模拟定时器
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <thread>       // 提供线程创建、管理等功能。
#include <chrono>       // 提供时间相关的功能，如计时器。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <atomic>       // 提供原子操作，用于多线程编程中的数据安全访问。
#include "GpioDef.hpp"
#include "KeyDef.hpp"
#include "Timer.hpp"

#define LED_PIN  (int)83
extern std::shared_ptr<GpioDef> led;
void toggle_led(void);
void terminal_print(void);

int test_thread_timer(void) {
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>(); // 创建一个键盘对象
    led = std::make_shared<GpioDef>(LED_PIN, MODE_OUT); // 创建一个GPIO对象，用于操作GPIO

    std::shared_ptr<Timer> tim_toggle_led = std::make_shared<Timer>(100, toggle_led, true); // 创建一个定时器对象，用于定时200ms翻转LED，并立即打开
    std::shared_ptr<Timer> tim_terminal_print = std::make_shared<Timer>(1000, terminal_print, true); // 创建一个定时器对象，用于定时500ms终端打印，并立即打开
    tim_toggle_led->setExitInfo("toggle_led线程已自动调用析构函数退出。"); // 设置toggle_led线程退出时的打印字符
    tim_terminal_print->setExitInfo("terminal_print线程已自动调用析构函数退出。"); // 设置terminal_print线程退出时的打印字符
    // tim_toggle_led->start(); // 启动这个tim_toggle_led定时器线程
    // tim_terminal_print->start(); // 启动这个tim_terminal_print定时器线程

    while (1) {
        if(kb->kbhit() && kb->readKey() == KEY_ESC) break; // 检测按下Esc键后退出循环
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 延时10ms
    }
    std::cout << "主线程已退出。" << std::endl;
    return 0;
}

std::shared_ptr<GpioDef> led;
void toggle_led(void) {
    led->setVal(!led->getVal());
}

uint64_t cnt = 0;
void terminal_print(void) {
    printf("终端打印第%ld次\n", cnt++);
}