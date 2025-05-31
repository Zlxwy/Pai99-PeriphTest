// 开机定时翻转久久派上的LED灯
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "GpioDef.hpp"
#include "test.hpp"

#define LED_PIN  (int)83

int test_toggle_led(void) {
    std::shared_ptr<GpioDef> led = std::make_shared<GpioDef>(LED_PIN, MODE_OUT);
    while(true) {
        led->setVal(!led->getVal());
        usleep(ms2us(500));
    }
}