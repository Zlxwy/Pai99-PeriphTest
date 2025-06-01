#include <iostream>
#include <unistd.h>
#include "test.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "MotorController.h"
#include "PwmController.h"

int test_pwm_output(void) {
    /*电机PWM*/
    GPIO mortorEN(73);
    constexpr int pwmchip[2] = { 8, 8 };
    constexpr int pwmnum[2] = { 2, 1 };
    constexpr int gpioNum[2] = { 12, 13 };
    constexpr int encoder_pwmchip[2] = { 0, 3 };
    constexpr int encoder_gpioNum[2] = { 75, 72 };
    constexpr int encoder_dir[2] = { 1, -1 };
    constexpr unsigned int period_ns = 50000; // 20 kHz
    MotorController motorL(pwmchip[0], pwmnum[0], gpioNum[0], period_ns, 0, 0, 0, 0, encoder_pwmchip[0], encoder_gpioNum[0], encoder_dir[0]);
    MotorController motorR(pwmchip[1], pwmnum[1], gpioNum[1], period_ns, 0, 0, 0, 0, encoder_pwmchip[1], encoder_gpioNum[1], encoder_dir[1]);
    
    /*键盘*/
    KeyDef keyboard;
    std::cout << "按ESC键退出" << std::endl;

    while (true) {
        if ( keyboard.kbhit() && keyboard.readKey()==KEY_ESC ) break;
        for(int i=0; i<100; i++) {
            motorL.updateduty(i),
            motorR.updateduty(i),
            usleep(ms2us(10));
        }
        for(int i=99; i>0; i--) {
            motorL.updateduty(i),
            motorR.updateduty(i),
            usleep(ms2us(10));
        }
    }
    return 0;
}