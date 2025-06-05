// 语音模块wonderEcho测试
// 按ESC退出
// 按W键切换声音开关（0xff/0x00）
// 按A/D键切换哪条声音（0x00~0xff）
// 0xFF 0x10 是“人行道前停车礼让行人”
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include "KeyDef.hpp"
#include "wonderEcho.h"
#include "test.hpp"

int test_wonderEcho_sound(void) {
    wonderEchoInit();
    usleep(sec2us(1)); // 等待1秒钟，确保模块初始化完成
    KeyDef kb;

    uint16 reg=0, data=1;
    // wonderEchoSend(0xff, 10);
    // usleep(sec2us(5));
    // std::cout << "声音2" << std::endl;
    // wonderEchoSend(0x00, 0x05);
    // usleep(sec2us(5));

    std::cout << "按ESC退出" << std::endl;
    while (true) {
        if (kb.kbhit()) {
            int getKey = kb.readKey();
            if (getKey == KEY_ESC) break;
            switch (getKey) {
                case KEY_W: case KEY_w: // 按下W键
                    if (reg == 0xff) reg = 0x00;
                    else if (reg == 0x00) reg = 0xff; // 限制寄存器值在0-100之间
                    break;
                case KEY_A: case KEY_a: // 按下A键
                    if (data == 1) data = data;
                    else if (data >= UINT8_MAX) data = UINT8_MAX;
                    else data = data - 1;
                    break;
                case KEY_D: case KEY_d: // 按下D键
                    if (data >= UINT8_MAX) data = UINT8_MAX;
                    else data = data + 1; // 限制数据值在0-100之间
                    break;
                default: goto slpTime; break;
            }
            std::cout << "reg: " << (int)reg << ", data: " << (int)data << std::endl;
            wonderEchoSend(reg, data); // 发送数据到 WonderEcho 模块
        }
        slpTime:usleep(ms2us(10));
    }
    return 0;
}