// 在终端打印有颜色的字符
#include <iostream>
#include <unistd.h>
#include "StylePrint.hpp"
#include "test.hpp"

int test_color_code(void) {
    stylePrint(FG_RED, "This is red text\n");
    stylePrint(BOLD+UNDERLINE+FG_GREEN, "This is green text with underline\n");
    stylePrint(BOLD+BLINK+FG_YELLOW, "This is yellow text with blink\n");
    return 0;
}
