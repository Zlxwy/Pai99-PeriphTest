// 摄像头获取图像，在终端上不断刷新显示字符画
#include <opencv2/opencv.hpp> // 包含OpenCV库的头文件，用于图像处理。
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <stdlib.h>     // 提供标准库函数，如 malloc 、 free 等，用于内存分配和释放。
#include <string>       // 提供 std::string 类，用于处理字符串，方便进行字符串操作，如拼接、查找等。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include <fstream>      // 用于文件流操作，可进行文件的读写。
#include <sstream>      // 提供字符串流功能，可在字符串和其他数据类型间转换。
#include <cstring>      // 包含许多操作C风格字符串（字符数组）的函数，如 strcpy 、 strlen 等。
#include <memory>       // 智能指针等内存管理功能，用于自动资源管理。
#include <vector>       // 提供动态数组功能，如 vector ，用于存储和管理数据集合。
#include <chrono>       // 提供时间相关的功能，如计时器。
#include "KeyDef.hpp"
#include "test.hpp"

int test_ocv_cam_disp_with_char(void) {
    /*键盘对象，用于按下ESC键后结束程序*/
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();

    cv::VideoCapture cam(CAM_INDEX); // 打开相机
    if ( !cam.isOpened() ) { // 如果无法打开相机
        std::cerr << "ERROR: 无法打开摄像头！" << std::endl;
        return -1;
    }
    cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_WIDTH); // 设置宽度
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT); // 设置高度
    std::cout << cam.get(cv::CAP_PROP_FRAME_WIDTH) << "*" << cam.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;

    // /*创建窗口用于图像显示*/
    // cv::namedWindow("gray Picture", cv::WINDOW_AUTOSIZE); // 创建一个窗口

    /*初始化帧数和时间戳*/
    int frameCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    double fps = 0;

    while (true) { // 开始循环
        /*帧率计算*/
        frameCount++; // 更新帧数
        auto currentTime = std::chrono::steady_clock::now(); // 获取当前时间
        double elapsedTime = std::chrono::duration<double>(currentTime - startTime).count(); // 计算时间差
        if (elapsedTime >= 1.0) { // 每秒更新一次帧率
            fps = frameCount / elapsedTime; // 计算帧率
            frameCount = 0; startTime = currentTime; // 重置帧数和时间戳
        }
        std::stringstream ss; // 创建一个字符串流
        ss << "FPS:" << std::fixed << std::setprecision(2) << fps; // FPS:xx.xx

        /*图像矩阵定义*/
        cv::Mat frame; // 原始图像
        cv::Mat grayFrame; // 灰度图
        cv::Mat grayResizedFrame; // 灰度图进行缩放后的图像

        /*图像获取*/
        bool ret = cam.read(frame); // 从相机获取新的一帧
        if (!ret) { // 如果没有图像
            std::cout << "ERROR: 无法获取摄像头画面！" << std::endl;
            break;
        }

        /*图像操作*/
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY); // 把原图转换为灰度图
        cv::resize(grayFrame, grayResizedFrame, cv::Size(80, 60)); // 把灰度图缩放至160x120
        cv::putText(grayFrame, ss.str(), cv::Point(0, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0,0,0), 2); // 写上帧率值

        /*终端显示图像*/
        clearScreen_ESCAPE; // 清空终端
        const char* chars = " .-:+=?%&#"; // 从黑到白，映射至0~255
        for (int i=0; i< grayResizedFrame.rows; i++) { // 遍历每一行
            for (int j=0; j< grayResizedFrame.cols; j++) {  // 遍历一行内的所有像素（列）
                int avg = grayResizedFrame.at<uint8>(i, j); // 获取该像素位置的灰度值
                int index = avg * 9 / 255;                  // 计算出对应字符的索引
                printf("%c%c", chars[index], chars[index]);  // 打印两个字符，刚好是个正方形
            }
            printf("\n"); // 在准备下一行时打印一个换行符
        }
        printf("\n%s\nOriginal Size: %d*%d\n", ss.str().c_str(), frame.cols, frame.rows); // 显示帧率、列数、行数

        // /*显示图像*/
        // cv::imshow("gray Picture", grayFrame); // 在对应窗口中显示原尺寸的灰度图像

        /*检测按键*/
        if ( kb->kbhit() && kb->readKey()==KEY_ESC ) break;
    }

    cam.release(); // 释放相机资源
    std::cout << "相机资源已释放" << std::endl; // 添加日志来跟踪资源释放
    // cv::destroyAllWindows(); // 关闭所有OpenCV创建的窗口
    // std::cout << "所有窗口已关闭" << std::endl; // 添加日志来跟踪窗口关闭

    return 0;
}