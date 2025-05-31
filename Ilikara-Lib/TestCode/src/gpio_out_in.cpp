// 测试GPIO输入输出
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "GpioDef.hpp"
#include "test.hpp"

#define  LED_PIN    (int)83
#define  KEY_PIN    (int)44

int test_gpio_out_in(void) {
    std::shared_ptr<GpioDef> led = std::make_shared<GpioDef>(LED_PIN, MODE_OUT);
    std::shared_ptr<GpioDef> key = std::make_shared<GpioDef>(KEY_PIN, MODE_IN);
    /*测试代码1：引脚电平间隔翻转*/
    while(true) {
        led->setVal(0);
        printf("gpio value 0\n");
        usleep(ms2us(300));
        led->setVal(1);
        printf("gpio value 1\n");
        usleep(ms2us(300));
    }
    // /*测试代码2：引脚电平间隔翻转*/
    // while(true) {
    //     led->setVal(!led->getVal());
    //     printf("gpio value %d\n", led->getVal());
    //     usleep(ms2us(200));
    // }
    // /*测试代码3：输出引脚电平跟随输入引脚电平*/
    // int val;
    // while(1) {
    //     val = key->getVal();
    //     led->setVal(val);
    //     printf("Input: %d\n", val);
    //     usleep(ms2us(100));
    // }
    return 0;
}