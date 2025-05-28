// Map the error to the control variable through PID operation for control
// 将误差经过PID计算得出控制量进行控制
// 按下空格键启动停止
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

#define CAR_DIRECTION   1  // 车子方向为前进
#define CAR_SPEED       22 // 车子速度（百分比）

int test_ocv_ctrl_car_with_pid(void) {
    /*实例化硬件对象*/
    std::shared_ptr<KeyDef> kb = std::make_shared<KeyDef>();
    std::shared_ptr<PwmDef> servoPwm = std::make_shared<PwmDef>("pwmchip1", "0", SERVO_CNT_MAX, angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX), "inversed", true);
    std::shared_ptr<PwmDef> motorPwm = std::make_shared<PwmDef>("pwmchip2", "0", MOTOR_CNT_MAX, speedPercent2cnt(0, MOTOR_CNT_MAX), "inversed", false);
    std::vector<std::shared_ptr<GpioDef>> motorDir;
    motorDir.push_back(std::make_shared<GpioDef>(12, MODE_OUT));
    motorDir.push_back(std::make_shared<GpioDef>(13, MODE_OUT));

    /*初始化硬件状态*/
    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); // 初始化舵机角度居中
    motorPwm->setDuty( speedPercent2cnt(CAR_SPEED, MOTOR_CNT_MAX) ); //初始化电机转动速度（高电平占空比25%）
    for(auto &dir: motorDir) dir->setVal(CAR_DIRECTION); // 前进方向

    /*打开相机并设置分辨率*/
    cv::VideoCapture cam(CAM_INDEX); // 打开相机
    if (!cam.isOpened()) { // 如果无法打开相机
        stylePrint(FG_RED+BOLD, "ERROR: 无法打开摄像头！\n");
        return -1;
    }
    cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_WIDTH); // 设置宽度
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT); // 设置高度
    std::cout << cam.get(cv::CAP_PROP_FRAME_WIDTH) << "*" << cam.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;

    /*初始化帧数和时间戳*/
    int frameCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    double fps = 0;

    /*图像矩阵定义*/
    cv::Mat frame;
    cv::Mat resizedFrame;
    cv::Mat grayFrame;
    cv::Mat binFrame;

    /*其他一些经典变量定义*/
    bool motorEn = true; // 用来控制电机是否使能
    // 关于变量xl的PID参数定义
    constexpr double xlMID = 0.0; // 直走的偏斜率
    constexpr double xlMIN = xlMID - 0.7; // 偏斜范围最小值
    constexpr double xlMAX = xlMID + 0.7; // 偏斜范围最大值
    constexpr double xlKp=25, xlKi=0, xlKd=48; // PID参数
    double xlPrevError = 0; // 上次的误差
    double xlCurrError = 0; // 当前的误差
    double xlErrorInteg = 0; // 误差的积分
    double xlErrorDeriv = 0; // 误差的微分
    double xlOutput = 0; // PID计算输出
    // 关于变量py的PID参数定义
    constexpr double pyMID = (0+RESIZED_WIDTH-1)/2.0; // 直走的偏移
    constexpr double pyMIN = pyMID - 32.0; // 偏移范围最小值
    constexpr double pyMAX = pyMID + 32.0; // 偏移范围最大值
    constexpr double pyKp=0.062, pyKi=0, pyKd=0.9; // PID参数
    double pyPrevError = 0; // 上次的误差
    double pyCurrError = 0; // 当前的误差
    double pyErrorInteg = 0; // 误差的积分
    double pyErrorDeriv = 0; // 误差的微分
    double pyOutput = 0; // PID计算输出
    // PID计算的最终输出
    double angleOffset = 0; // PID计算输出
    double finalAngle = 0; // 施加到舵机上的角度值

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
        // drawLeftRightBlackBorder(binFrame); // 在二值图像的四周画黑线

        /***********使用B站up主“村东头无敌的小瞎子”(uid:385282905)的一种滤波算法***********/
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
        double xl = calAverSlopeFromRowToRow(resizedFrame, midPoint, ROW_UP, ROW_DOWN); // 计算中线的偏斜
        double py = calAverXCoordFromRowToRow(resizedFrame, midPoint, ROW_UP, ROW_DOWN); // 计算X坐标平均值

        /********************根据计算出来的斜率，控制硬件驱动****************/
        xl = myClamp(xl, xlMIN, xlMAX); // 限制偏斜的范围
        py = myClamp(py, pyMIN, pyMAX); // 限制偏移的范围
        // PID计算，xl (begin)--------------------------------|
        xlCurrError = xl - xlMID; // （当前误差）如果寻线向左偏的话，xlCurrError将会是个正数，此时需要输出为正数，因此kp和ki都为正数
        xlErrorInteg += xlCurrError; // （误差累加）
        xlErrorDeriv = xlCurrError - xlPrevError; // （误差微分）如果寻线向左偏又回正了一点的话，xlErrorDeriv将会是个负数，此时需要输出为负数，因此kd为正数
        xlOutput = xlKp*xlCurrError + xlKi*xlErrorInteg + xlKd*xlErrorDeriv; // PID输出
        xlPrevError = xlCurrError; // （先前误差）记录这一次的误差，作为下一次再次进入的先前误差
        // PID计算，py (begin)--------------------------------|
        pyCurrError = pyMID - py; // （当前误差）如果寻线向左偏移的话，pyCurrError将会是个正数，此时需要输出为正数，因此kp和ki都为正数
        pyErrorInteg += pyCurrError; // （误差累加）
        pyErrorDeriv = pyCurrError - pyPrevError; // （误差微分）如果寻线向左偏移又回正了一点的话，pyErrorDeriv将会是个负数，此时需要输出为负数，因此kd为正数
        pyOutput = pyKp*pyCurrError + pyKi*pyErrorInteg + pyKd*pyErrorDeriv; // PID输出
        pyPrevError = pyCurrError; // （先前误差）记录这一次的误差，作为下一次再次进入的先前误差
        // PID计算 (end)--------------------------------------|
        angleOffset = xlOutput + pyOutput; // 将两个PID输出相加，得到舵机的转角偏移值
        angleOffset = myClamp(angleOffset, -SERVO_OFFSET_ANGLE_MAX, +SERVO_OFFSET_ANGLE_MAX); // 限制舵机转角偏移值的范围
        finalAngle = SERVO_ANGLE_MID + angleOffset; // 在中值 SERVO_ANGLE_MID 的基础之上
        servoPwm->setDuty( angle2cnt(finalAngle, SERVO_CNT_MAX) ); // 将PID输出加载到舵机

        /************************终端打印信息**************************/
        clearScreen_ESCAPE; // 清空终端
        std::cout << "帧率：" << ss_fps.str() << std::endl;
        std::cout << "阈值：" << th << std::endl;
        std::cout << "偏斜：" << xl << std::endl;
        std::cout << "偏移：" << py << std::endl;
        std::cout << "转向：" << ((xl<-0.2) ? "向右" : ((xl>0.2)?"向左":"直走")) << std::endl;
        std::cout << "角度：" << finalAngle << " (" << angle2cnt(finalAngle, SERVO_CNT_MAX) << ")" << std::endl;

        /******************************检测按键***********************************/        
        if ( kb->kbhit() ) {
            int keyGet = kb->readKey();
            if( keyGet == KEY_ESC ) break;
            else if (keyGet == KEY_SPACE) {
                motorPwm->setCmd(motorEn); //翻转电机使能状态
                motorEn = !motorEn;
            }
        }
    }

    cam.release(); // 释放相机资源
    std::cout << "相机资源已释放" << std::endl; // 添加日志来跟踪资源释放

    servoPwm->setDuty( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) );
    motorPwm->setCmd(false);
    usleep(ms2us(1000));

    return 0;
}