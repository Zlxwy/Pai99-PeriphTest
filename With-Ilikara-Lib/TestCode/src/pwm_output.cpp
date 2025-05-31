#include <iostream>
#include <unistd.h>
#include "test.hpp"
#include "KeyDef.hpp"
#include "GPIO.h"
#include "MotorController.h"
#include "PwmController.h"

int main(void) {
    /*电机PWM*/
    GPIO mortorEN(73);
    const int pwmchip[2] = { 8, 8 };
    const int pwmnum[2] = { 2, 1 };
    const int gpioNum[2] = { 12, 13 };
    const int encoder_pwmchip[2] = { 0, 3 };
    const int encoder_gpioNum[2] = { 75, 72 };
    const int encoder_dir[2] = { 1, -1 };
    const unsigned int period_ns = 50000; // 20 kHz
    MotorController motorL(pwmchip[0], pwmnum[0], gpioNum[0], period_ns,
        0, 0, 0, 0,
        encoder_pwmchip[0], encoder_gpioNum[0], encoder_dir[0]
    );
    MotorController motorR(pwmchip[1], pwmnum[1], gpioNum[1], period_ns,
        0, 0, 0, 0,
        encoder_pwmchip[1], encoder_gpioNum[1], encoder_dir[1]
    );
    
    /*键盘*/
    KeyDef key;
    std::cout << "按ESC键退出" << std::endl;

    while (true) {
        if (key.kbhit()&&key.readKey()==KEY_ESC) break;
        for(int i=0; i<period_ns; i+=period_ns/100) {
            motorL.updateduty(i),
            motorR.updateduty(i),
            usleep(ms2us(10));
        }
        for(int i=period_ns-1; i>0; i-=period_ns/100) {
            motorL.updateduty(i),
            motorR.updateduty(i),
            usleep(ms2us(10));
        }
    }
    return 0;
}