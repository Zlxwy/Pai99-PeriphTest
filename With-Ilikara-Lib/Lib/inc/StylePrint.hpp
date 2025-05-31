#ifndef __STYLEPRINT_HPP
#define __STYLEPRINT_HPP
#include <iostream>
#include <string>

// ANSI 转义码用于设置颜色和样式
#define STYLE_RST           std::string("\033[0m") // 重置所有属性
#define BOLD                std::string("\033[1m") // 粗体
#define FAINT               std::string("\033[2m") // 微弱
#define ITALIC              std::string("\033[3m") // 斜体
#define UNDERLINE           std::string("\033[4m") // 下划线
#define BLINK               std::string("\033[5m") // 闪烁
#define REVERSE             std::string("\033[7m") // 反显
#define HIDDEN              std::string("\033[8m") // 隐藏
#define STRIKETHROUGH       std::string("\033[9m") // 删除线
#define FG_BLACK            std::string("\033[30m")
#define FG_RED              std::string("\033[31m")
#define FG_GREEN            std::string("\033[32m")
#define FG_YELLOW           std::string("\033[33m")
#define FG_BLUE             std::string("\033[34m")
#define FG_MAGENTA          std::string("\033[35m")
#define FG_CYAN             std::string("\033[36m")
#define FG_WHITE            std::string("\033[37m")
#define BG_BLACK            std::string("\033[40m")
#define BG_RED              std::string("\033[41m")
#define BG_GREEN            std::string("\033[42m")
#define BG_YELLOW           std::string("\033[43m")
#define BG_BLUE             std::string("\033[44m")
#define BG_MAGENTA          std::string("\033[45m")
#define BG_CYAN             std::string("\033[46m")
#define BG_WHITE            std::string("\033[47m")

void stylePrint(const std::string& style, const std::string& text);
// 使用示例：stylePrint(BOLD+UNDERLINE+FG_GREEN, "This is green text with underline\n");

#endif // #ifndef __STYLEPRINT_HPP