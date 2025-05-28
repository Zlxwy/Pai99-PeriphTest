// 测试信号触发模拟定时中断(500ms)
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include <stdlib.h>     // 提供标准库函数，如内存分配、程序控制等。
#include <signal.h>     // 提供信号处理函数，如 signal 和 sigaction 。
#include <sys/time.h>   // 提供定时器设置函数，如 setitimer 和 getitimer 。
#include "test.hpp"

void timer_handler(int signum);

int test_signal_sim_timer_interrupt(void) {
    signal(SIGALRM, timer_handler); // 注册信号处理函数
    struct itimerval timer;
    timer.it_value.tv_sec = 2;              // 设置初始延迟 2s
    timer.it_value.tv_usec = 0;             // 设置初始延迟 0us
    timer.it_interval.tv_sec = 0;           // 设置间隔延迟 0s
    timer.it_interval.tv_usec = ms2us(500); // 设置间隔延迟 500ms
    // 设置定时器，若设置失败则直接退出程序
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("无法设置定时器");
        return 0;
    }
    printf("Waiting for timer...\n");
    while(1) {
        printf("定时中断正在运行中！\n");
        sleep(1);
    }
    return 0;
}

// 信号处理函数，相当于定时中断ISR
void timer_handler(int signum) {
    static uint64_t val = 0;
    printf("进入定时函数第 %ld 次\n", val++);
    // 添加自定义逻辑
}