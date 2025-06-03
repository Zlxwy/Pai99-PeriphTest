#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <memory>
#include "KeyDef.hpp"
#include "test.hpp"

template <typename T>
T readNumFromFile(const std::string& filename) {
    /*如果确定文件中的内容总是有效的数值，并且对性能有较高要求，那么这种方法更合适*/
    std::ifstream file(filename);
    T num = 0;
    if (file.is_open()) file >> num; // 读取文件中的值
    else std::cerr << "Failed to open " << filename << std::endl;
    return num;
}

int test_read_num_from_file(void) {
    KeyDef kb;
    while (true) {
        if(kb.kbhit() && kb.readKey() == KEY_ESC) break;
        clearScreen_ESCAPE;
        std::cout << "Range: " << readNumFromFile<uint16_t>("/home/root/myExecs/about_vl53l0x/range_datas/RangeMilliMeter.txt") << "(mm)" << std::endl;
        usleep(ms2us(30));
    }
    return 0;
}
