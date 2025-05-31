#ifndef __TIMER_HPP
#define __TIMER_HPP
#include <iostream>
#include <functional>
#include <thread>
#include <atomic>

// 如何创建一个定时线程？
// 一、比如创建一个定时线程：定时10ms调用function函数，并在创建完毕后立即打开
// Timer tim_thread(10, function, true);
// 二、比如创建一个定时线程，定时10ms调用function函数，在创建完毕后手动打开
// Timer tim_thread(10, function, false);
// tim_thread.start();
// 定时线程会在析构时自动退出，也可以手动调用stop方法退出

class Timer {
public:
    Timer(int interval_ms, std::function<void(void)> task, bool isOnImdtly);
    ~Timer();

    void start(void); // 创建一个worker线程，在线程内调用run方法
    void stop(void); // 将原子布尔变量置false，线程将退出
    void setExitInfo(const std::string &info); // 设置定时线程退出后在终端打印的信息

private:
    void run(void);
    int interval; // 定时时间，单位为毫秒
    std::function<void(void)> task; // 定时执行的任务函数
    std::atomic<bool> isRunning; // 线程标志位，置false线程就会结束运行
    std::thread worker; // 用于创建一个循环运行的线程
    std::string exitInfo; // 定时线程退出后在终端打印的信息
};

#endif // #ifndef __TIMER_HPP