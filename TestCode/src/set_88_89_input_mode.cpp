// 自己魔改了一下久久派的引脚连接，将GPIO66短接到GPIO88和GPIO89上了，因此需要把GPIO88和GPIO89设置为输入模式
#include <iostream>
#include <unistd.h>
#include <GpioDef.hpp>
int set_88_88_input_mode(void) {
    GpioDef gpio88 = GpioDef(88, MODE_IN);
    GpioDef gpio89 = GpioDef(89, MODE_IN);
    while (true) {
        // printf("GPIO88: %d, GPIO89: %d\n", gpio88.getVal(), gpio89.getVal());
        usleep(500*1000); // 循环内延时500ms
    }
}