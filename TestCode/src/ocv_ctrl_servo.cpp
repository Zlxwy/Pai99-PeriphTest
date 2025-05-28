// 简单识别一下赛道（第一个程序）
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
#include "StylePrint.hpp"
#include "GpioDef.hpp"
#include "KeyDef.hpp"
#include "PwmDef.hpp"
#include "test.hpp"

int test_ocv_ctrl_servo(void) {
    /*键盘对象，用于按下ESC键后结束程序*/
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    /*舵机 (GPIO65): PWM控制器1，计数周期SERVO_CNT_MAX，设置初始占空值angle2cnt(90, SERVO_CNT_MAX)，“inversed”，初始化完直接启动*/
    std::shared_ptr<PwmDef> servoPwm = std::make_shared<PwmDef>("pwmchip1", "0", SERVO_CNT_MAX, angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX), "inversed", true);
    
    /*打开相机并设置分辨率*/
    cv::VideoCapture cam(CAM_INDEX); // 打开相机
    if (!cam.isOpened()) { // 如果无法打开相机
        std::cerr << "ERROR: 无法打开摄像头！" << std::endl;
        return -1;
    }
    cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_WIDTH); // 设置宽度
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT); // 设置高度
    std::cout << cam.get(cv::CAP_PROP_FRAME_WIDTH) << "*" << cam.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;

    // /*创建窗口用于图像显示*/
    // cv::namedWindow("缩放后的彩色图", cv::WINDOW_NORMAL); // 创建一个窗口
    // cv::namedWindow("原二值化图像", cv::WINDOW_NORMAL); // 创建一个窗口
    // cv::namedWindow("处理后的二值化图像", cv::WINDOW_NORMAL); // 创建一个窗口

    /*初始化帧数和时间戳*/
    int frameCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    double fps = 0;

    /*图像矩阵定义*/
    cv::Mat frame;
    cv::Mat resizedFrame;
    cv::Mat grayFrame;
    cv::Mat binFrame;

    while (true) { // 开始循环
        /*************************帧率计算*******************************/
        frameCount++; // 更新帧数
        auto currentTime = std::chrono::steady_clock::now(); // 获取当前时间
        double elapsedTime = std::chrono::duration<double>(currentTime - startTime).count(); // 计算时间差
        if (elapsedTime >= 1.0) { // 每秒更新一次帧率
            fps = frameCount / elapsedTime; // 计算帧率
            frameCount = 0; startTime = currentTime; // 重置帧数和时间戳
        }
        std::stringstream ss_fps; // 创建一个字符串流
        ss_fps << "FPS:" << std::fixed << std::setprecision(2) << fps; // FPS:xx.xx

        /*************************图像获取*******************************/
        bool ret = cam.read(frame); // 从相机获取新的一帧
        if (!ret) { // 如果没有图像
            stylePrint(FG_RED+BOLD, "ERROR: 无法获取摄像头画面\n");
            break;
        }

        /************************图像缩放、灰度化、二值化******************* */
        double th = bgr2bin(frame, resizedFrame, grayFrame, binFrame, cv::Size(RESIZED_WIDTH, RESIZED_HEIGHT), GRAY2BIN_THRESH, 255); // 自定义阈值
        drawLeftRightBlackBorder(binFrame); // 在二值图像的四周画黑线

        /***********************将小块的黑色区域填充为黑色**************************/
        // 反转小面积黑白区域方法1
        // invertSmallRegion_1(binFrame, MIN_AREA_THRESH); // 反转面积小于MIN_AREA_THRESH的轮廓

        // 反转小面积黑白区域方法2
        // invertSmallRegion_2(binFrame, MIN_AREA_THRESH); // 反转面积小于MIN_AREA_THRESH的轮廓

        // 使用一种滤波算法
        binImgFilter(binFrame);

        /************边线识别：识别到的边界点坐标存放入两个容器中*************/
        std::vector<cv::Point> leftEdgePoints, rightEdgePoints;  // 定义两个容器来存储左右边缘点
        findEdgePix(binFrame, leftEdgePoints, rightEdgePoints); // 查找二值图像的左右边界像素点坐标，存入两个容器中

        /**********************在彩色图上画出这些识别到的边界点，以及中线************************/
        size_t min_dop_num = std::min<size_t>(leftEdgePoints.size(), rightEdgePoints.size()); // 获取这两个容器的元素数量最小值
        std::vector<cv::Point> midPoint; // 用于收集两条边线之间的中点坐标值
        for (size_t i = 0; i < min_dop_num; i++) {
            midPoint.push_back(calMidPoint(leftEdgePoints.at(i), rightEdgePoints.at(i))); // 加入这个中点
            resizedFrame.at<cv::Vec3b>(leftEdgePoints.at(i)) = cv::Vec3b(BGR_BLUE); // 画左边线
            resizedFrame.at<cv::Vec3b>(midPoint.back()) = cv::Vec3b(BGR_GREEN); // 画左右边线之间的中点线，即循迹线
            resizedFrame.at<cv::Vec3b>(rightEdgePoints.at(i)) = cv::Vec3b(BGR_RED); // 画右边线
        }
        
        /**********************计算在某两行之间中点的斜率*************************/
        double xl = calAverSlopeFromRowToRow(resizedFrame, midPoint, 52, 68);

        /***********************显示图像，龙芯上没有图像界面显示********************/
        // cv::imshow("缩放后的彩色图", resizedFrame); // 在对应窗口中显示图像
        // cv::imshow("处理后的二值化图像", binFrame); // 在对应窗口中显示图像

        /************************终端打印信息**************************/
        clearScreen_ESCAPE;
        std::cout << "帧率：" << ss_fps.str() << std::endl;
        std::cout << "阈值：" << th << std::endl;
        std::cout << "斜率：" << xl << std::endl;
        std::cout << "转向：" << ((xl < -0.2) ? "向右" : ((xl > 0.2) ? "向左" : "直走")) << std::endl;
        //====================================================================================================|

        /********************根据计算出来的斜率，控制舵机转角****************/
        /*将斜率限制在[-0.5,+0.5](右到左)之间，映射至角度[-15,+15](右到左)*/
        double Cxl = myClamp(xl, -0.5, 0.5); // Clamped xl 限制后的斜率
        double CangleOffset = mapArea(-0.5, 0.5, -SERVO_OFFSET_ANGLE_MAX, +SERVO_OFFSET_ANGLE_MAX, Cxl); // 映射到[-SERVO_OFFSET_ANGLE_MAX,+SERVO_OFFSET_ANGLE_MAX]之间)
        CangleOffset = myClamp(CangleOffset, -SERVO_OFFSET_ANGLE_MAX, SERVO_OFFSET_ANGLE_MAX); // Clamped angleOffset 限制后的角度偏移
        servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID+CangleOffset, SERVO_CNT_MAX) ); // 设置舵机角度
        std::cout << "舵机角度：" << SERVO_ANGLE_MID+CangleOffset
                  << "(" << angle2cnt(SERVO_ANGLE_MID+CangleOffset, SERVO_CNT_MAX) << ")"
                  << std::endl;

        /******************************检测按键***********************************/
        // int KeyGet = cv::waitKey(1); // 等待1ms获取按键值
        // if (KeyGet == KEY_ESC) break; // 如果按下Esc的话，退出循环
        
        if ( kb->kbhit() ) { // 检测到有按键输入
            int keyGet = kb->readKey(); // 获取键值
            if( keyGet == KEY_ESC ) break; // 键值是Esc的则退出程序
        }
    }

    cam.release(); // 释放相机资源
    std::cout << "相机资源已释放" << std::endl; // 添加日志来跟踪资源释放
    // cv::destroyAllWindows(); // 关闭所有OpenCV创建的窗口
    // std::cout << "所有窗口已关闭" << std::endl; // 添加日志来跟踪窗口关闭

    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) );
    usleep(ms2us(1000));

    return 0;
}