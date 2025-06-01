#include <iostream>
#include <unistd.h>
#include "test.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "PwmController.h"

/*测试PWM输出*/
int test_pwm_output(void) {
    /*初始化电机PWM: [0]pwmchip8,pwm2(GPIO89) 和 [1]pwmchip8,pwm1(GPIO88)*/
    PwmController motorPWM[2] = { PwmController(8,2), PwmController(8,1) }; // 电机PWM控制器
    for (auto &mp: motorPWM) {
        mp.setPolarity(false); // 设置极性为反向
        mp.setPeriod(MOTOR_CNT_MAX); // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz
        mp.setDutyCycle( speedPercent2cnt(0, MOTOR_CNT_MAX) ); // 设置初始占空比为0
        mp.enable(); // 启动电机PWM输出
    } // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz

    /*键盘*/
    KeyDef keyboard;

    /*打印提示信息*/
    std::cout << "按ESC键退出程序" << std::endl;

    while (true) {
        for(int i=0; i<100; i++) { // 逐渐点亮
            for (auto &mp: motorPWM) mp.setDutyCycle( speedPercent2cnt(i, MOTOR_CNT_MAX) );
            usleep(ms2us(10));
        }
        for(int i=99; i>0; i--) { // 逐渐熄灭
            for (auto &mp: motorPWM) mp.setDutyCycle( speedPercent2cnt(i, MOTOR_CNT_MAX) );
            usleep(ms2us(20));
        }
        usleep(ms2us(2000)); // 暂停2秒
        
        if ( keyboard.kbhit() ) {
            int getKey = keyboard.readKey();
            if (getKey == KEY_ESC) break;
        }
    }
    
    return 0;
}