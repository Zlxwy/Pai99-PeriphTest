// 这个程序用来看看舵机的最大转动角度
// 按键盘A和键盘D控制舵机转到左右最大
// 按键盘逗号和键盘句号控制舵机微调
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "StylePrint.hpp"
#include "KeyDef.hpp"
#include "PwmController.h"
#include "test.hpp"

int test_keyboard_ctrl_servo(void) {
    /*初始化舵机PWM: pwmchip1,pwm0(GPIO65)*/
    PwmController servoPwm = PwmController(1,0); // 舵机PWM控制器
    servoPwm.setPeriod(SERVO_CNT_MAX); // 设置PWM周期为SERVO_CNT_MAX，对应频率为50Hz
    servoPwm.setDutyCycle(angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX)); // 设置初始占空比为中间位置
    servoPwm.setPolarity(false); // 设置极性为反向
    bool is_servo_enabled = false; // 舵机使能状态
    (is_servo_enabled) ? servoPwm.enable() : servoPwm.disable(); // 启动或禁用舵机PWM输出

    /*键盘*/
    KeyDef keyboard;
    
    stylePrint(BOLD+UNDERLINE+FG_YELLOW, "按逗号键，舵机控制前轮左转；按句号键，舵机控制前轮右转\n");
    double servo_angle = SERVO_ANGLE_MID;
    
    while (true) {
        if ( keyboard.kbhit() ) {
            int getKey = keyboard.readKey();
            if (getKey == KEY_ESC) break; // 如果是Esc就退出程序
            switch (getKey) {
                case KEY_SPACE: // 空格键
                    is_servo_enabled = !is_servo_enabled; // 切换舵机使能状态
                    (is_servo_enabled) ? servoPwm.enable() : servoPwm.disable(); // 启动或禁用舵机PWM输出
                    break;
                case KEY_A: case KEY_a: // 逗号
                    servo_angle = SERVO_ANGLE_MAX; // 左转
                    break;
                case KEY_D: case KEY_d: // 句号
                    servo_angle = SERVO_ANGLE_MIN; // 右转
                    break;
                case 44: // 逗号
                    servo_angle += 1.0; // 左转
                    break;
                case 46: // 句号
                    servo_angle -= 1.0; // 右转
                    break;
                default:
                    servo_angle = SERVO_ANGLE_MID;
                    break;
            }
            servo_angle = myClamp(servo_angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
            servoPwm.setDutyCycle( angle2cnt(servo_angle, SERVO_CNT_MAX) );
            stylePrint(BOLD, "舵机角度：");
            std::cout << servo_angle << std::endl;
        }
        usleep(ms2us(20));
    }
    servoPwm.disable(); // 禁用舵机PWM输出
    return 0;
}