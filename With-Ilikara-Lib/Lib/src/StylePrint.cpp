#include "StylePrint.hpp"

/*以指定风格输出字符串，指定风格就行，会自动复位风格*/
void stylePrint(const std::string& style, const std::string& text) {
    std::cout << style << text << STYLE_RST;
}
