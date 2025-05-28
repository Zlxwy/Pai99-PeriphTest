// 测试多线程模拟定时中断(500ms)
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <thread>       // 线程类相关
#include <chrono>       // 时间类相关
#include "test.hpp"

void timer_handler(void) {
    uint64_t cnt = 0;
    while (true) {
        printf("进入定时函数第 %ld 次\n", cnt++);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int test_thread_sim_timer_interrupt(void) {
    std::shared_ptr<std::thread> timer_thread = std::make_shared<std::thread>(timer_handler);
    timer_thread->detach();
    uint64_t cnt = 0;
    while(1) {
        printf("定时中断正在运行中！---------------------------------------%ld|\n", cnt++);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}