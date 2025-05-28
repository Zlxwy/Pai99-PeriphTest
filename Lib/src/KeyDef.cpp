#include "KeyDef.hpp"

// 启用非阻塞模式
void KeyDef::enable_nonblock_mode(void) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate); // 获取当前终端属性
    ttystate.c_lflag &= ~(ICANON | ECHO); // 关闭规范模式和回显
    ttystate.c_cc[VMIN] = 1; // 设置最小字符数
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate); // 设置终端属性
}

// 禁用非阻塞模式
void KeyDef::disable_nonblock_mode(void) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate); // 获取当前终端属性
    ttystate.c_lflag |= ICANON | ECHO; // 开启规范模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate); // 设置终端属性
}

KeyDef::KeyDef(void) {
    this->enable_nonblock_mode(); // 启用非阻塞输入模式
}

KeyDef::~KeyDef() {
    this->disable_nonblock_mode(); // 禁用非阻塞输入模式
}

// 检测键盘是否有输入
int KeyDef::kbhit(void) {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

int KeyDef::readKey(void) {
    return getchar();
}