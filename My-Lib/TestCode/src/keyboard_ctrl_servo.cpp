// 这个程序用来看看舵机的最大转动角度
// 按键盘A和键盘D控制舵机转到左右最大
// 按键盘逗号和键盘句号控制舵机微调
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include "StylePrint.hpp"
#include "KeyDef.hpp"
#include "PwmDef.hpp"
#include "test.hpp"

int test_keyboard_ctrl_servo(void) {
    /*键盘对象，用于按下ESC键后结束程序*/
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    /*舵机 (GPIO65): PWM控制器1，计数周期PWM_CLK_FREQ/SERVO_PWM_FREQ，设置初始占空值angle2cnt(90, PWM_CLK_FREQ/SERVO_PWM_FREQ)，“inversed”，初始化完直接启动*/
    std::shared_ptr<PwmDef> servoPwm = std::make_shared<PwmDef>("pwmchip1", "0", SERVO_CNT_MAX, angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX), "inversed", true);
    
    stylePrint(BOLD+UNDERLINE+FG_YELLOW, "按逗号键，舵机控制前轮左转\n按句号键，舵机控制前轮右转\n");
    double servo_angle = SERVO_ANGLE_MID;
    
    while (true) {
        if (kb->kbhit()) {
            int c = kb->readKey();
            if (c == KEY_ESC) break; // 如果是Esc就退出程序
            switch (c) {
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
            servoPwm->setDuty( angle2cnt(servo_angle, SERVO_CNT_MAX) );

        }
        clearScreen_ESCAPE;
        stylePrint(BOLD, "舵机角度：");
        std::cout << servo_angle << std::endl;
        usleep(ms2us(20));
    }
    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) );
    usleep(ms2us(1000));
    return 0;
}