#include <iostream>
#include <unistd.h>
#include "KeyDef.hpp"
#include "wonderEcho.h"
#include "test.hpp"

int test_wonderEcho_sound(void) {
    wonderEchoInit();
    KeyDef kb;

    while (true) {
        wonderEchoSend(0x00, 0x01); // 发出一段声音，暂不知道是啥
        usleep(sec2us(3));
        wonderEchoSend(0x00, 0x05); // 发出一段声音，暂不知道是啥
        usleep(sec2us(3));
        if (kb.kbhit()) {
            int getKey = kb.readKey();
            if (getKey == KEY_ESC) break;
        }
    }
    
    return 0;
}