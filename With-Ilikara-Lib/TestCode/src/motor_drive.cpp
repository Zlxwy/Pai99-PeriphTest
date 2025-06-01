#include <iostream>
#include <unistd.h>
#include <memory>
#include <array>
#include "test.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "MotorController.h"
#include "PwmController.h"

int test_motor_drive(void) {
    /*初始化传入参数*/
    constexpr int pwmchip[2] = { 8, 8 };
    constexpr int pwmnum[2] = { 2, 1 };
    constexpr int gpioNum[2] = { 12, 13 };
    constexpr int encoder_pwmchip[2] = { 0, 3 };
    constexpr int encoder_gpioNum[2] = { 75, 72 };

    /*初始化电机PWM*/
    PwmController motorPWM[2] = { PwmController(pwmchip[0], pwmnum[0]), PwmController(pwmchip[1], pwmnum[1]) }; // 电机PWM控制器
    for (auto &mp: motorPWM) { mp.setPeriod(MOTOR_CNT_MAX); /*mp.enable();*/ } // 设置PWM周期为MOTOR_CNT_MAX，对于频率为20kHz
    
    /*初始化电机方向引脚*/
    GPIO motorDIR[2] = { GPIO(gpioNum[0]), GPIO(gpioNum[1]) }; // 电机方向GPIO
    for (auto &md: motorDIR) { md.setDirection("out"); md.setValue(true); } // 设置GPIO方向为输出，并输出高电平

    /*初始化电机使能引脚*/
    GPIO motorEN = GPIO(73); // 电机使能GPIO
    motorEN.setDirection("out"); // 设置GPIO方向为输出
    motorEN.setValue(true); // 设置GPIO输出高电平，启用电机
    
    /*初始化编码器*/
    ENCODER motorENCODER[2] = { ENCODER(encoder_pwmchip[0], encoder_gpioNum[0]), ENCODER(encoder_pwmchip[1], encoder_gpioNum[1]) }; // 电机编码器

    /*键盘*/
    KeyDef keyboard;
    std::cout << "按ESC键退出程序" << std::endl;

    while (true) {
        if ( keyboard.kbhit() && keyboard.readKey()==KEY_ESC ) break;
        for(int i=0; i<100; i++) {
            for (auto &mp: motorPWM) mp.setDutyCycle(speedPercent2cnt(i, MOTOR_CNT_MAX));
            usleep(ms2us(10));
        }
        for(int i=99; i>0; i--) {
            for (auto &mp: motorPWM) mp.setDutyCycle(speedPercent2cnt(i, MOTOR_CNT_MAX));
            usleep(ms2us(10));
        }
        usleep(ms2us(2000)); // 暂停1秒
    }
    return 0;
}