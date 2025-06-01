#include <iostream>
#include <unistd.h>
#include "test.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "PwmController.h"
#include "encoder.h"

/*测试电机整体功能*/
int test_motor_drive(void) {
    // /*电机PWM*/
    // GPIO mortorEN(73);
    // constexpr int pwmchip[2] = { 8, 8 };
    // constexpr int pwmnum[2] = { 2, 1 };
    // constexpr int gpioNum[2] = { 12, 13 };
    // constexpr int encoder_pwmchip[2] = { 0, 3 };
    // constexpr int encoder_gpioNum[2] = { 75, 72 };
    // constexpr int encoder_dir[2] = { 1, -1 };
    // constexpr unsigned int period_ns = 50000; // 20 kHz
    // MotorController motorL(pwmchip[0], pwmnum[0], gpioNum[0], period_ns, 0, 0, 0, 0, encoder_pwmchip[0], encoder_gpioNum[0], encoder_dir[0]);
    // MotorController motorR(pwmchip[1], pwmnum[1], gpioNum[1], period_ns, 0, 0, 0, 0, encoder_pwmchip[1], encoder_gpioNum[1], encoder_dir[1]);

    /*初始化电机PWM: [0]pwmchip8,pwm2(GPIO89)和[1]pwmchip8,pwm1(GPIO88)*/
    PwmController motorPWM[2] = { PwmController(8,2), PwmController(8,1) }; // 电机PWM控制器
    for (auto &mp: motorPWM) { mp.setPeriod(MOTOR_CNT_MAX); mp.disable(); } // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz

    /*初始化电机方向引脚: [0]GPIO12和[1]GPIO13*/
    GPIO motorDIR[2] = { GPIO(12), GPIO(13) }; // 电机方向GPIO
    for (auto &md: motorDIR) { md.setDirection("out"); md.setValue(true); } // 设置GPIO方向为输出，并输出高电平

    /*初始化电机使能引脚: GPIO73*/
    GPIO motorEN = GPIO(73); // 电机使能GPIO
    motorEN.setDirection("out"); // 设置GPIO方向为输出
    motorEN.setValue(true); // 设置GPIO输出高电平，启用电机

    /*初始化舵机PWM: pwmchip1,pwm0(GPIO65)*/
    PwmController servoPwm = PwmController(1,0); // 舵机PWM控制器
    servoPwm.setPeriod(SERVO_CNT_MAX); // 设置PWM周期为SERVO_CNT_MAX，对应频率为50Hz
    servoPwm.setDutyCycle(angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX)); // 设置初始占空比为中间位置
    servoPwm.setPolarity(true); // 设置极性为正向
    servoPwm.enable(); // 启动舵机PWM输出
    
    /*初始化编码器: [0]lsb:pwmchip0(GPIO64),dir:GPIO75, [1]lsb:pwmchip3(GPIO67),dir:GPIO72*/
    ENCODER motorENCODER[2] = { ENCODER(0,75), ENCODER(3,72) }; // 电机编码器

    /*键盘*/
    KeyDef keyboard;

    /*打印提示信息*/
    std::cout << "按ESC键退出程序" << std::endl;

    while (true) {
        usleep(ms2us(1000)); // 暂停2秒
    }
    return 0;
}