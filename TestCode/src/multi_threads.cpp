// 多线程：一个线程翻转LED，一个线程呼吸灯，一个线程打印终端
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <thread>       // 提供线程创建、管理等功能。
#include <chrono>       // 提供时间相关的功能，如计时器。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <atomic>       // 提供原子操作，用于多线程编程中的数据安全访问。
#include "GpioDef.hpp"
#include "PwmDef.hpp"
#include "KeyDef.hpp"

extern std::atomic<bool> shouldExit;
void exitAllThreads(void);
void thread_toggle_led(void);
void thread_breath_LED(void);
void thread_terminal_print(void);

int test_multi_threads(void) {
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    std::shared_ptr<std::thread> toggle_led = std::make_shared<std::thread>(thread_toggle_led);
    std::shared_ptr<std::thread> breath_led = std::make_shared<std::thread>(thread_breath_LED);
    std::shared_ptr<std::thread> terminal_print = std::make_shared<std::thread>(thread_terminal_print);
    /*如果要使用线程的join方法阻塞等待线程的话，就不能使用detach方法将线程分离*/
    // toggle_led->detach();
    // breath_led->detach();
    // terminal_print->detach();
    uint64_t cnt = 0;
    while(1) {
        if(kb->kbhit() && kb->readKey() == KEY_ESC) {
            exitAllThreads();
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "thread main has exited" << std::endl;
    /*阻塞等待所有线程退出*/
    if( toggle_led->joinable() ) toggle_led->join();
    if( breath_led->joinable() ) breath_led->join();
    if( terminal_print->joinable() ) terminal_print->join();
    return 0;
}

std::atomic<bool> shouldExit(false);
void exitAllThreads(void) { shouldExit.store(true); }

#define LED_PIN  (int)83
void thread_toggle_led(void) {
    std::shared_ptr<GpioDef> led = std::make_shared<GpioDef>(LED_PIN, MODE_OUT);
    while( !shouldExit.load() ) {
        led->setVal(!led->getVal());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "thread toggle_led has exited" << std::endl;
}

void thread_breath_LED(void) {
    /*一个周期总共计数100000个(1GHz计数频率)，一个周期内占空计数100个，占空计数时为高电平，启动*/
    std::shared_ptr<PwmDef> pwmPin = std::make_shared<PwmDef>("pwmchip2", "0", 100000, 100, "inversed", true);
    while( !shouldExit.load() ) {
        for(int i=0; i<100000; i+=1000)
            pwmPin->setDuty(i),
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        for(int i=99999; i>0; i-=1000)
            pwmPin->setDuty(i),
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "thread breath_LED has exited." << std::endl;
}

void thread_terminal_print(void) {
    uint64_t cnt = 0;
    while ( !shouldExit.load() ) {
        printf("终端打印第%ld次\n", cnt++);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "thread terminal_print has exited." << std::endl;
}