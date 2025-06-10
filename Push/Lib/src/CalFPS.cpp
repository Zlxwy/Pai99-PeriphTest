#include "CalFPS.hpp"

CalFPS::CalFPS(void) {
    this->frameCount = 0; // 初始化帧数
    this->startTime = std::chrono::steady_clock::now(); // 初始化时间戳
    this->fpsNum = 0.0; // 初始化帧率
}

CalFPS::~CalFPS(void) {
    // 析构函数，当前不需要释放资源
    std::cout << "CalFPS对象已销毁" << std::endl; // 添加日志来跟踪对象销毁
}

double CalFPS::updateFPS(void) {
    this->frameCount++; // 更新帧数
    auto currentTime = std::chrono::steady_clock::now(); // 获取当前时间
    double elapsedTime = std::chrono::duration<double>(currentTime - this->startTime).count(); // 计算时间差
    if (elapsedTime >= 1.0) { // 每秒更新一次帧率
        this->fpsNum = this->frameCount / elapsedTime; // 计算帧率
        this->frameCount = 0; // 重置帧数
        this->startTime = currentTime; // 重置帧数和时间戳
    }
    std::stringstream ss_fps; // 创建一个字符串流
    ss_fps << std::fixed << std::setprecision(2) << fpsNum; // xx.xx(保留两位小数)
    this->fpsStr = ss_fps.str(); // 将格式化后的字符串存储到fpsStr中
    return this->fpsNum; // 返回当前帧率
}
