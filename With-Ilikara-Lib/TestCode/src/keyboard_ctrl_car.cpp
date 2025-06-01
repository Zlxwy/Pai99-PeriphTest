#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "test.hpp"
#include "StylePrint.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "PwmController.h"

/*测试电机整体功能*/
int test_keyboard_ctrl_car(void) {
    /*初始化电机PWM: [0]pwmchip8,pwm2(GPIO89)和[1]pwmchip8,pwm1(GPIO88)*/
    PwmController motorPWM[2] = { PwmController(8,2), PwmController(8,1) }; // 电机PWM控制器
    for (auto &mp: motorPWM) {
        mp.setPeriod(MOTOR_CNT_MAX);
        mp.setDutyCycle( speedPercent2cnt(0, MOTOR_CNT_MAX) ); // 设置初始占空比为0
        mp.disable();
    } // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz

    /*初始化电机方向引脚: [0]GPIO12和[1]GPIO13*/
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
    PwmController servoPwm = PwmController(1,0); // 舵机PWM控制器
    servoPwm.setPeriod(SERVO_CNT_MAX); // 设置PWM周期为SERVO_CNT_MAX，对应频率为50Hz
    servoPwm.setDutyCycle( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); // 设置初始占空比为中间位置
    servoPwm.setPolarity(false); // 设置极性为反向
    servoPwm.enable(); // 启动舵机PWM输出

    /*键盘*/
    KeyDef keyboard;

    /*打印提示信息*/
    stylePrint(FG_RED+BOLD, "按ESC键退出程序\n");
    stylePrint(FG_YELLOW+UNDERLINE, "W: 前进, S: 后退, A: 左转, D: 右转, 其他键: 停止\n");

    while (true) {
        for(int i=0; i<100; i++) {
            for (auto &mp: motorPWM) mp.setDutyCycle(speedPercent2cnt(i, MOTOR_CNT_MAX));
            usleep(ms2us(10));
        }
        for(int i=99; i>0; i--) {
            for (auto &mp: motorPWM) mp.setDutyCycle(speedPercent2cnt(i, MOTOR_CNT_MAX));
            usleep(ms2us(10));
        }
        usleep(ms2us(2000)); // 暂停1秒

        if ( keyboard.kbhit() ) {
            int getKey = keyboard.readKey();
            if (getKey == KEY_ESC) break;
            printf("> %c\n", (char)getKey); // 打印按下按键
            switch(getKey) {
                case KEY_W: case KEY_w: // W: 前进
                    servoPwm.setDutyCycle( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); //控制舵机居中（15度）
                    for (auto &md: motorDIR) md.setValue(true); //设置电机转动方向
                    for (auto &mp: motorPWM) {
                        mp.setDutyCycle( speedPercent2cnt(20, MOTOR_CNT_MAX) ); //设置电机转动速度
                        mp.enable(); //启动电机PWM输出
                    }
                    break;
                case KEY_S: case KEY_s: // S: 后退
                    servoPwm.setDutyCycle( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); //控制舵机居中
                    for (auto &md: motorDIR) md.setValue(false); //设置电机转动方向
                    for (auto &mp: motorPWM) {
                        mp.setDutyCycle( speedPercent2cnt(20, MOTOR_CNT_MAX) ); //设置电机转动速度
                        mp.enable(); //启动电机PWM输出
                    }
                    break;
                case KEY_A: case KEY_a: // A: 左转
                    servoPwm.setDutyCycle( angle2cnt(SERVO_ANGLE_MAX, SERVO_CNT_MAX) ); //控制舵机打左
                    break;
                case KEY_D: case KEY_d: // D: 右转
                    servoPwm.setDutyCycle( angle2cnt(SERVO_ANGLE_MIN, SERVO_CNT_MAX) ); //控制舵机打右
                    break;
                default: // 其他键
                    for (auto &mp: motorPWM) {
                        mp.setDutyCycle( speedPercent2cnt(0, MOTOR_CNT_MAX) ); //设置电机转动速度
                        mp.disable(); //关闭电机PWM输出
                    }
                    break;
            }
        }
        usleep(ms2us(10));
    }
    return 0;
}