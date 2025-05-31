#ifndef __KEYDEF_HPP
#define __KEYDEF_HPP
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

constexpr auto KEY_ESC = 27;
#define KEY_SPACE   32
#define KEY_W       87
#define KEY_S       83
#define KEY_A       65
#define KEY_D       68
#define KEY_w       119
#define KEY_s       115
#define KEY_a       97
#define KEY_d       100
#define KEY_0       48
#define KEY_1       49
#define KEY_2       50
#define KEY_3       51
#define KEY_4       52
#define KEY_5       53
#define KEY_6       54
#define KEY_7       55
#define KEY_8       56
#define KEY_9       57
#define KEY_COMMA   44 // 英文逗号
#define KEY_DOT     46 // 英文句号
#define KEY_SLASH   47 // 斜杠
#define KEY_BACKSLASH 92 // 反斜杠

class KeyDef {
private:
    void enable_nonblock_mode(void);
    void disable_nonblock_mode(void);
public:
    KeyDef(void);
    int kbhit(void); //KeyBoard Hit 是否有按键被按下
    int readKey(void);
    ~KeyDef();
};

#endif // #ifndef __KEYDEF_HPP