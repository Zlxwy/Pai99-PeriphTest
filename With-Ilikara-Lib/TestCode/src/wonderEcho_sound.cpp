#include <iostream>
#include <unistd.h>
#include "KeyDef.hpp"
#include "wonderEcho.h"
#include "test.hpp"

int test_wonderEcho_sound(void) {
    wonderEchoInit();
    usleep(sec2us(1)); // 等待1秒钟，确保模块初始化完成
    KeyDef kb;

    std::cout << "声音1" << std::endl;
    wonderEchoSend(0x00, 0x01);
    usleep(sec2us(5));
    std::cout << "声音2" << std::endl;
    wonderEchoSend(0x00, 0x05);
    usleep(sec2us(5));

    std::cout << "按ESC退出" << std::endl;
    while (true) {
        if (kb.kbhit()) {
            int getKey = kb.readKey();
            if (getKey == KEY_ESC) break;
        }
    }
    return 0;
}