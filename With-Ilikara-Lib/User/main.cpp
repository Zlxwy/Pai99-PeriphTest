#include "main.hpp"
#include "test.hpp"

int main(void) {
    // test_motor_drive(); // 测试电机驱动
    // test_pwm_output(); // 测试PWM输出
    // test_gpio_output(); // 测试GPIO输出
    // test_read_num_from_file(); // 测试从文件中读取数值
    // test_read_distance(); // 测试vl53l0x距离传感器测量数值读取
    test_wonderEcho_sound(); // 测试wonderEcho声音模块

    // test_keyboard_ctrl_servo(); // 测试键盘控制舵机
    // test_keyboard_ctrl_car(); // 测试键盘控制小车
    // test_ocv_ctrl_car_with_pid(); // 测试PID控制小车
}
