// 测试4个PWM引脚输出
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "PwmDef.hpp"
#include "KeyDef.hpp"
#include "test.hpp"

int test_pwm_output(void) {
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    /*一个周期总共计数100000个(1GHz计数频率)，一个周期内占空计数100个，占空计数时为高电平，启动*/
    std::shared_ptr<PwmDef> pwmPin64 = std::make_shared<PwmDef>("pwmchip0", "0", 100000, 100, "inversed", true);
    std::shared_ptr<PwmDef> pwmPin65 = std::make_shared<PwmDef>("pwmchip1", "0", 100000, 100, "inversed", true);
    std::shared_ptr<PwmDef> pwmPin66 = std::make_shared<PwmDef>("pwmchip2", "0", 100000, 100, "inversed", true);
    std::shared_ptr<PwmDef> pwmPin67 = std::make_shared<PwmDef>("pwmchip3", "0", 100000, 100, "inversed", true);
    while(true) {
        if(kb->kbhit()&&kb->readKey()==KEY_ESC) break;
        for(int i=0; i<100000; i+=1000)
            pwmPin64->setDuty(i),
            pwmPin65->setDuty(i),
            pwmPin66->setDuty(i),
            pwmPin67->setDuty(i),
            usleep(ms2us(10));
        for(int i=99999; i>0; i-=1000)
            pwmPin64->setDuty(i),
            pwmPin65->setDuty(i),
            pwmPin66->setDuty(i),
            pwmPin67->setDuty(i),
            usleep(ms2us(10));
    }
    return 0;
}