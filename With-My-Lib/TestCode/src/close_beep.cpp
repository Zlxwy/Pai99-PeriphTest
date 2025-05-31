// 因为龙邱拓展板上的蜂鸣器一开机就会响，开机关闭龙邱科技拓展板上的蜂鸣器
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "GpioDef.hpp"
#include "test.hpp"

#define BEEP_PIN  (int)61

int test_close_beep(void) {
    std::shared_ptr<GpioDef> beep = std::make_shared<GpioDef>(BEEP_PIN, MODE_OUT);
    beep->setVal(0);
    while(true) {
        usleep(ms2us(500));
    }
    return 0;
}