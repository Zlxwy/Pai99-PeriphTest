// 键盘控制龙芯车，W前进，S后退，A左转，D右转，空格：翻转LED，其他键：停止
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <vector>       // 提供动态数组功能，如 vector ，用于存储和管理数据集合。
#include <thread>       // 提供线程创建、管理等功能。
#include <chrono>       // 提供时间相关的功能，如计时器。
#include "KeyDef.hpp"
#include "GpioDef.hpp"
#include "PwmDef.hpp"
#include "test.hpp"

int test_keyboard_ctrl_car(void) {
    //键盘控制，WSAD控制前后左右
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    //LED，按一下空格键翻转，看看系统是否还在运行中 (GPIO83)
    std::shared_ptr<GpioDef> led = std::make_shared<GpioDef>(83, MODE_OUT);
    //舵机 (GPIO65): PWM控制器1，计数周期SERVO_CNT_MAX，设置初始占空值angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX)，“inversed”，初始化完直接启动
    std::shared_ptr<PwmDef> servoPwm = std::make_shared<PwmDef>("pwmchip1", "0", SERVO_CNT_MAX, angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX), "inversed", true);
    //电机PWM，控制转速 (GPIO66): PWM控制器2，计数周期MOTOR_CNT_MAX，设置初始占空值speedPercent2cnt(0, MOTOR_CNT_MAX)，“inversed”，初始化完后不启动（等会手动开启）
    std::shared_ptr<PwmDef> motorPwm = std::make_shared<PwmDef>("pwmchip2", "0", MOTOR_CNT_MAX, speedPercent2cnt(0, MOTOR_CNT_MAX), "inversed", false);
    //电机DIR，控制转向 (GPIO12, GPIO13)
    std::vector<std::shared_ptr<GpioDef>> motorDir;
    motorDir.push_back(std::make_shared<GpioDef>(12, MODE_OUT));
    motorDir.push_back(std::make_shared<GpioDef>(13, MODE_OUT));
    for(auto &dir: motorDir) dir->setVal(1);
    //打印提示信息
    std::cout << "Press key (W, S, A, D) to control the car (ESC to exit)" << std::endl;
    std::cout << "W: Forward, S: Backwaed, A: Left, D: Right, Other: Stop" << std::endl;
    while(true) {
        if ( kb->kbhit() ) { // 是否有键盘输入
            int c = kb->readKey(); // 读取字符
            if (c == KEY_ESC) break; // 如果是Esc就退出程序
            printf("The key you have pressed is: %c\n", (char)c); // 打印按下按键
            switch(c) { // 根据按键执行相应操作
                case KEY_SPACE: // 空格键：控制LED闪烁
                    led->setVal( !led->getVal() );
                    break;
                case KEY_W: case KEY_w: // W: 前进
                    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); //控制舵机居中（15度）
                    for(auto &dir: motorDir) dir->setVal(1); //设置电机转动方向
                    motorPwm->setCmd(true); //启动电机PWM输出
                    motorPwm->setDuty( speedPercent2cnt(20, MOTOR_CNT_MAX) ); //设置电机转动速度（高电平占空比25%）
                    break;
                case KEY_S: case KEY_s: // S: 后退
                    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); //控制舵机居中
                    for(auto &dir: motorDir) dir->setVal(0); //设置电机转动方向
                    motorPwm->setCmd(true); //启动电机PWM输出
                    motorPwm->setDuty( speedPercent2cnt(20, MOTOR_CNT_MAX) ); //设置电机转动速度（高电平占空比15%）
                    break;
                case KEY_A: case KEY_a: // A: 左转
                    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MAX, SERVO_CNT_MAX) ); //控制舵机打左
                    break;
                case KEY_D: case KEY_d: // D: 右转
                    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MIN, SERVO_CNT_MAX) ); //控制舵机打右
                    break;
                default: // 其他键
                    motorPwm->setCmd(false); //关闭电机PWM输出
                    break;
            }
        }
        // else { // 如果没有键盘输入，则控制舵机居中
        //     servoPwm->setDuty( angle2cnt(90, PWM_CLK_FREQ/SERVO_PWM_FREQ) ); //控制舵机居中
        // }
        usleep(10);
    }
    motorPwm->setCmd(false);
    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); //控制舵机居中
    usleep(1000);
    return 0;
}
