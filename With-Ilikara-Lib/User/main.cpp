#include "main.hpp"
#include "test.hpp"

int main(void) {
    // test_pwm_output(); // 测试PWM输出（已验证）
    // test_gpio_output(); // 测试GPIO输出（已验证）
    // test_read_num_from_file(); // 测试从文件中读取数值（已验证）
    // test_read_distance(); // 测试vl53l0x距离传感器测量数值读取（已验证）
    // test_wonderEcho_sound(); // 测试wonderEcho声音模块（未验证成功，可能是排线问题）

    // test_motor_drive(); // 测试电机驱动（已验证）
    // test_keyboard_ctrl_servo(); // 测试键盘控制舵机（已验证）
    // test_keyboard_ctrl_car(); // 测试键盘控制小车（已验证）
    // test_ocv_ctrl_car_with_pid(); // 测试PID控制小车（已验证）
    test_vl53l0x_obtc_avds(); // 测试避障模式下的PID控制小车（已验证，效果不好）
}
