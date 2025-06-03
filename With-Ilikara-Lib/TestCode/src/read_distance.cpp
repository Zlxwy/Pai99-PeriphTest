#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <cstdint>
#include "KeyDef.hpp"
#include "test.hpp"

/**
 * @brief 这个程序是从文件中读取vl53l0x测量的距离数值，
 *        因此在运行此程序之前，先要运行一下"~/myExecs/about_vl53l0x/run_vl53l0x.sh"脚本文件，启动vl53l0x测距程序。
 * @attention 不要用\r，不知道为啥用\r会很卡。
 */
int test_read_distance(void) {
    KeyDef kb;
    while (true) {
        clearScreen_ESCAPE;
        std::cout << "Range: "
                  << std::dec << std::setw(5) << std::setfill(' ')
                  << readNumFromFile<uint16>("/home/root/myExecs/about_vl53l0x/range_datas/RangeMilliMeter.txt") << "(mm)"
                  << std::endl;
        if(kb.kbhit() && kb.readKey() == KEY_ESC) break;
        usleep(ms2us(30));
    }
    return 0;
}
