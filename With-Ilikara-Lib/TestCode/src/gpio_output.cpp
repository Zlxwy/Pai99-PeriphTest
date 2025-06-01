#include <iostream>
#include <unistd.h>
#include "GPIO.h"
#include "KeyDef.hpp"
#include "test.hpp"

#define LED_GPIO_PIN  83

/*测试GPIO输出*/
int test_gpio_output(void) {
    /*LED输出: GPIO83*/
    GPIO led(LED_GPIO_PIN);
    led.setDirection("out"); // 设置GPIO方向为输出
    led.setValue(true); // 设置GPIO输出高电平（LED高电平熄灭，低电平点亮）

    /*键盘*/
    KeyDef keyboard;

    /*打印提示信息*/
    std::cout << "按ESC键退出程序" << std::endl;

    while (true) {
        led.setValue( !led.readValue() );
        usleep(ms2us(200)); // 每500毫秒切换一次LED状态

        // led.setValue(false);
        // usleep(ms2us(200));
        // led.setValue(true);
        // usleep(ms2us(800));

        if ( keyboard.kbhit() ) {
            int getKey = keyboard.readKey();
            if (getKey == KEY_ESC) break;
        }
    }
    return 0;
}