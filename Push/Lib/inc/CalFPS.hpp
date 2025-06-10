#ifndef __CALFPS_HPP_
#define __CALFPS_HPP_
#include <iostream>
#include <chrono>       // 提供时间相关的功能，如计时器。
#include <sstream>      // 提供字符串流功能，可在字符串和其他数据类型间转换。
#include <iomanip>      // 提供格式化输出功能

// 如何使用？
// CalFPS fpsCnt;
//
// /*使用方式1*/
// while (true) {
//     fpsCnt.updateFPS(); // 更新帧数
//     // 其他处理逻辑...
//     printf("当前帧率: %.2f\n", fpsCnt.fps); // 获取并打印当前帧率
//     printf("当前帧率: %s\n", fpsCnt.fpsStr.c_str()); // 打印格式化的帧率字符串
// }
//
// /*使用方式2*/
// while (true) {
//     // 其他处理逻辑...
//     printf("当前帧率: %.2f\n", fpsCnt.updateFPS()); // 更新帧数并获取当前帧率
//     printf("当前帧率: %s\n", fpsCnt.fpsStr.c_str()); // 打印格式化的帧率字符串
// }

class CalFPS {
public:
    /*初始化帧数和时间戳*/
    int frameCount; // 帧数计数器，在调用方法update()时递增
    std::chrono::_V2::steady_clock::time_point startTime;
    double fpsNum; // 帧率，在调用方法getFPS()时返回
    std::string fpsStr; // 帧率字符串，用于格式化输出 xx.xx(保留两位小数)

    CalFPS(void);
    ~CalFPS(void);
    double updateFPS(void);
};

#endif