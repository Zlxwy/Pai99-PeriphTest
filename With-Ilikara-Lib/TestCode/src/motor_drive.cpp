#include <iostream>
#include <unistd.h>
#include "test.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "PwmController.h"
#include "encoder.h"

/*电机整体功能初始化代码*/
int test_motor_drive(void) {
    /*初始化电机PWM: [0]pwmchip8,pwm2(GPIO89) 和 [1]pwmchip8,pwm1(GPIO88)*/
    PwmController motorPWM[2] = { PwmController(8,2), PwmController(8,1) }; // 电机PWM控制器
    for (auto &mp: motorPWM) {
        mp.setPolarity(false); // 设置极性为反向
        mp.setPeriod(MOTOR_CNT_MAX); // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz
        mp.setDutyCycle( speedPercent2cnt(0, MOTOR_CNT_MAX) ); // 设置初始占空比为0
        mp.enable(); // 启动电机PWM输出
    } // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz

    /*初始化电机方向引脚: [0]GPIO12 和 [1]GPIO13*/
    GPIO motorDIR[2] = { GPIO(12), GPIO(13) }; // 电机方向GPIO
    for (auto &md: motorDIR) {
        md.setDirection("out");
        md.setValue(true);
    } // 设置GPIO方向为输出，并输出高电平

    /*初始化电机使能引脚: GPIO73*/
    GPIO motorEN = GPIO(73); // 电机使能GPIO
    motorEN.setDirection("out"); // 设置GPIO方向为输出
    motorEN.setValue(true); // 设置GPIO输出高电平，启用电机

    /*初始化舵机PWM: pwmchip1,pwm0(GPIO65)*/
    PwmController servoPWM = PwmController(1,0); // 舵机PWM控制器
    servoPWM.setPolarity(false); // 设置极性为反向
    servoPWM.setPeriod(SERVO_CNT_MAX); // 设置PWM周期为SERVO_CNT_MAX，对应频率为50Hz
    servoPWM.setDutyCycle(angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX)); // 设置初始占空比为中间位置
    servoPWM.enable(); // 启动舵机PWM输出
    
    /*初始化编码器: [0]lsb:pwmchip0(GPIO64),dir:GPIO75, [1]lsb:pwmchip3(GPIO67),dir:GPIO72*/
    ENCODER motorENCODER[2] = { ENCODER(0,75), ENCODER(3,72) }; // 电机编码器

    /*键盘*/
    KeyDef keyboard;

    /*打印提示信息*/
    std::cout << "按ESC键退出程序" << std::endl;

    while (true) {
        // 在前面的初始化代码，已经调用过enable()方法启动了PWM输出，
        for (auto &mp: motorPWM) mp.setDutyCycle(speedPercent2cnt(25, MOTOR_CNT_MAX)); // 设置电机速度
        for (auto &md: motorDIR) md.setValue(true); //设置电机正转向
        usleep(sec2us(3)); // 延时3秒
        for (auto &md: motorDIR) md.setValue(false); //设置电机反转向
        usleep(sec2us(3)); // 延时3秒
        for (auto &mp: motorPWM) mp.enable(); // 启动电机PWM输出
        for (auto &mp: motorPWM) mp.disable(); // 停止电机PWM输出

        // 在前面的初始化代码，已经调用过enable()方法启动了舵机PWM输出，
        servoPWM.setDutyCycle(angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX)); // 控制舵机居中
        usleep(sec2us(1)); // 暂停1秒
        servoPWM.setDutyCycle(angle2cnt(SERVO_ANGLE_MAX, SERVO_CNT_MAX)); // 控制舵机到最大角度
        usleep(sec2us(1)); // 暂停1秒
        servoPWM.setDutyCycle(angle2cnt(SERVO_ANGLE_MIN, SERVO_CNT_MAX)); // 控制舵机到最小角度
        usleep(sec2us(1)); // 暂停1秒
        servoPWM.enable(); // 启动舵机PWM输出
        servoPWM.disable(); // 停止舵机PWM输出

        if (keyboard.kbhit()) {
            int getKey = keyboard.readKey();
            if (getKey == KEY_ESC) break; // 按ESC键退出
        }

        usleep(ms2us(10)); // 暂停10ms
    }
    return 0;
}