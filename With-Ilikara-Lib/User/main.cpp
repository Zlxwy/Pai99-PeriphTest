#include "main.hpp"
#include "test.hpp"

int main(void) {
    // test_pwm_output(); // 测试PWM输出
    // test_gpio_output(); // 测试GPIO输出
    // test_motor_drive(); // 测试电机驱动
    // test_keyboard_ctrl_car(); // 测试键盘控制小车
    test_keyboard_ctrl_servo(); // 测试键盘控制舵机
}
