// obstacle avoidance with vl53l0x
// 使用 vl53l0x 传感器进行障碍物避让
// 按下空格键启动停止，按下 ESC 键退出
#include <opencv2/opencv.hpp> // 包含OpenCV库的头文件，用于图像处理。
#include <iostream>     // 提供输入输出流对象，如 cin 和 cout ，用于控制台输入输出。
#include <stdio.h>      // 提供标准输入输出函数，如 printf 和 scanf ，用于控制台输入输出。
#include <stdlib.h>     // 提供标准库函数，如 malloc 、 free 等，用于内存分配和释放。
#include <string>       // 提供 std::string 类，用于处理字符串，方便进行字符串操作，如拼接、查找等。
#include <unistd.h>     // 包含许多Unix标准系统调用和函数，像 read 、 write 、 close 等，用于文件操作、进程控制等。
#include <fstream>      // 用于文件流操作，可进行文件的读写。
#include <cstring>      // 包含许多操作C风格字符串（字符数组）的函数，如 strcpy 、 strlen 等。
#include "StylePrint.hpp"
#include "PIDObject.hpp"
#include "GPIO.h"
#include "PwmController.h"
#include "KeyDef.hpp"
#include "CalFPS.hpp"
#include "test.hpp"

#define CAR_DIRECTION ((bool)1)
#define CAR_SPEED     ((int)22) // 车子速度（百分比）
#define DISTANCE_FILE  std::string("/home/root/myExecs/about_vl53l0x/range_datas/RangeMilliMeter.txt")

int test_obtc_avds_with_vl53l0x(void) {
    /*初始化舵机PWM: pwmchip1,pwm0(GPIO65)*/
    PwmController servoPWM = PwmController(1,0); // 舵机PWM控制器
    servoPWM.setPolarity(false); // 设置极性为反向 "inversed"
    servoPWM.setPeriod(SERVO_CNT_MAX); // 设置PWM周期为SERVO_CNT_MAX，对应频率为50Hz
    servoPWM.setDutyCycle( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) ); // 设置初始占空比为中间位置
    servoPWM.enable(); // 启动舵机PWM输出

    /*初始化电机PWM: [0]pwmchip8,pwm2(GPIO89) 和 [1]pwmchip8,pwm1(GPIO88)*/
    bool isMotorEnabled = false; // 电机使能状态
    PwmController motorPWM[2] = { PwmController(8,2), PwmController(8,1) }; // 电机PWM控制器
    for (auto &mp: motorPWM) {
        mp.setPolarity(false); // 设置极性为反向 "inversed"
        mp.setPeriod(MOTOR_CNT_MAX); // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz
        mp.setDutyCycle( speedPercent2cnt(CAR_SPEED, MOTOR_CNT_MAX) ); // 设置初始占空比为CAR_SPEED
        (isMotorEnabled) ? mp.enable() : mp.disable(); // 启动或禁用电机PWM输出
    } // 设置PWM周期为MOTOR_CNT_MAX，对应频率为20kHz

    /*初始化电机方向引脚: [0]GPIO12 和 [1]GPIO13*/
    GPIO motorDIR[2] = { GPIO(12), GPIO(13) }; // 电机方向GPIO
    for (auto &md: motorDIR) {
        md.setDirection("out");
        md.setValue(CAR_DIRECTION);
    } // 设置GPIO方向为输出，并输出使电机前进的电平

    /*初始化电机使能引脚: GPIO73*/
    GPIO motorEN = GPIO(73); // 电机使能GPIO
    motorEN.setDirection("out"); // 设置GPIO方向为输出
    motorEN.setValue(true); // 设置GPIO输出高电平，启用电机
    
    /*键盘keyboard、帧率计数器fpsCnt*/
    KeyDef kb; // 键盘对象，用于检测按键事件
    CalFPS fpsCnt; // 帧率计数器对象，用于计算帧率

    /*打开相机并设置分辨率*/
    cv::VideoCapture cam(CAM_INDEX); // 打开相机
    if (!cam.isOpened()) { // 如果无法打开相机
        stylePrint(FG_RED+BOLD, "ERROR: 无法打开摄像头！\n");
        return -1;
    }
    cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_WIDTH); // 设置宽度
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT); // 设置高度
    std::cout << cam.get(cv::CAP_PROP_FRAME_WIDTH) << "*" << cam.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;

    /*图像矩阵定义*/
    cv::Mat frame;
    cv::Mat resizedFrame;
    cv::Mat grayFrame;
    cv::Mat binFrame;

    // 关于变量xl的PID参数定义
    PidObject xlPID;
        xlPID.isPolOfMeaValCsstWithOutVal = true;
        xlPID.kP = 29.9;
        xlPID.kI = 0.0;
        xlPID.kD = 76.0;
        xlPID.isFirstOrderFilterEnabled = true;
        xlPID.filterParam = 0.3;
        xlPID.targetVal = 0.0;
        xlPID.measuredVal = 0.0;
        xlPID.isErrorLimitEnabled = true;
        xlPID.errorLimit[0] = -0.7;
        xlPID.errorLimit[1] = +0.7;
        xlPID.isIntegLimitEnabled = true;
        xlPID.integLimit[0] = -20.0;
        xlPID.integLimit[1] = +20.0;
        xlPID.isOutputEnabled = true;
    // 关于变量py的PID参数定义
    PidObject pyPID;
        pyPID.isPolOfMeaValCsstWithOutVal = false;
        pyPID.kP = 0.03;
        pyPID.kI = 0.0;
        pyPID.kD = 0.9;
        pyPID.isFirstOrderFilterEnabled = false;
        pyPID.filterParam = 0.0;
        pyPID.targetVal = (0+RESIZED_WIDTH-1)/2.0;
        pyPID.measuredVal = 0.0;
        pyPID.isErrorLimitEnabled = true;
        pyPID.errorLimit[0] = -30.0;
        pyPID.errorLimit[1] = +30.0;
        pyPID.isIntegLimitEnabled = true;
        pyPID.integLimit[0] = -500.0;
        pyPID.integLimit[1] = +500.0;
        pyPID.isOutputEnabled = true;
    // PID计算的最终输出
    double angleOffset = 0; // PID计算输出
    double finalAngle = 0; // 施加到舵机上的角度值

    // 障碍物距离，从文件/home/root/myExecs/about_vl53l0x/range_datas/RangeMilliMeter.txt中读取
    uint16 od = 0; // Obstacle Distance
    constexpr uint16 odgj = 600;
    int jlgjCnt = 0; // 距离过近计数
    int jlzcCnt = 0; // 距离正常计数
    constexpr int jlgjCntMAX = 5; // 距离过近计数最大值
    constexpr int jlzcCntMAX = 3; // 距离正常计数最大值
    bool isOnAvds = false; // is on avoidance 是否在避障模式

    while (true) { // 开始循环
        /********************************帧率计算**********************************/
        fpsCnt.updateFPS(); // 更新帧率计数器

        /******************************检测按键***********************************/        
        if ( kb.kbhit() ) {
            int getKey = kb.readKey();
            if( getKey == KEY_ESC ) break;
            else if (getKey == KEY_SPACE) {
                isMotorEnabled = !isMotorEnabled; //翻转电机使能状态
                for (auto &mp: motorPWM) (isMotorEnabled)?mp.enable():mp.disable();
                jlgjCnt = 0; // 每次按下空格键时，重置距离过近计数器
                jlzcCnt = 0; // 每次按下空格键时，重置距离正常计数器
                isOnAvds = false; // 每次按下空格键时，重置避障模式状态
            }
        }

        /*************************障碍物距离获取*******************************/
        od = readNumFromFile<uint16>(DISTANCE_FILE); // 从文件中读取障碍物距离
        if (!isOnAvds && (od<odgj && od>220) && jlgjCnt<jlgjCntMAX) {
        // 如果不在避障模式，且距离过近，距离过近还没到最大值
            jlgjCnt++; // 距离过近计数器加1
        } else if (!isOnAvds && (od<odgj && od>220) && jlgjCnt>=jlgjCntMAX) {
        // 如果不在避障模式，且距离过近，距离过近已经达到最大值
            jlgjCnt = 0; // 距离过近计数器重置为0
            isOnAvds = true; // 开启避障模式
        } else if (!isOnAvds) jlgjCnt = 0;
        else if (isOnAvds && (od>=odgj || od < 220) && jlzcCnt<jlzcCntMAX) {
        // 如果在避障模式，且距离大于过近距离，距离正常还没到最大值
            jlzcCnt++; // 距离正常计数器加1
        } else if (isOnAvds && (od>=odgj || od<220) && jlzcCnt>=jlzcCntMAX) {
        // 如果在避障模式，且距离大于过近距离，距离正常已经达到最大值
            jlzcCnt = 0; // 距离正常计数器重置为0
            isOnAvds = false; // 关闭避障模式
        } else if (isOnAvds) jlzcCnt = 0;

        /*************************获取一张图像，转成黑白图像，并找出双边线的点坐标************************/
        bool ret = cam.read(frame); // 从相机获取新的一帧
        if (!ret) { // 如果没有图像
            stylePrint(FG_RED+BOLD, "ERROR: 无法获取摄像头画面\n");
            break;
        }
        bgr2bin(
            frame, resizedFrame, grayFrame, binFrame, // 输入图像，输出图像，灰度图，二值图
            cv::Size(RESIZED_WIDTH, RESIZED_HEIGHT), // 缩放后的图像大小
            GRAY2BIN_THRESH, 255 // 自定义阈值
        );
        binImgFilter(binFrame); // 使用B站up主“村东头无敌的小瞎子”(uid:385282905)的一种滤波算法
        std::vector<cv::Point> leftEdgePoints, rightEdgePoints;  // 定义两个容器来存储左右边缘点
        findEdgePix(binFrame, leftEdgePoints, rightEdgePoints); // 查找二值图像的左右边界像素点坐标，存入两个容器中

        if (!isOnAvds) { // 如果不在避障模式，就根据双边线计算出的中线循迹
            /**********************在彩色图上画出这些识别到的边界点，以及中线************************/
            size_t min_dop_num = std::min<size_t>(leftEdgePoints.size(), rightEdgePoints.size()); // 获取这两个容器的元素数量最小值
            std::vector<cv::Point> midPoint; // 用于收集两条边线之间的中点坐标值
            for (size_t i=0; i<min_dop_num; i++) { // 遍历左右边界点容器，计算出中点坐标
                midPoint.push_back(
                    calMidPoint(leftEdgePoints.at(i),rightEdgePoints.at(i))
                ); // 加入这个中点
            }

            /********************在两行之间计算出斜率xl和偏斜py，由此计算输出控制量****************/
            std::vector<cv::Point> midPointFiltered; // 用于存储对X坐标值滤波后的中点坐标
            filterXCoord(midPoint, midPointFiltered, 7); // 对中点坐标的X值进行滤波
            xlPID.measuredVal = calAverSlopeFromRowToRow(resizedFrame, midPointFiltered, ROW_UP, ROW_DOWN); // 计算中线的偏斜
            pyPID.measuredVal = calAverXCoordFromRowToRow(resizedFrame, midPointFiltered, ROW_UP, ROW_DOWN); // 计算X坐标平均值
        }
        else { // 如果在避障模式，就根据中线的位置，固定转向个角度
            std::vector<cv::Point> leftEdgePointsFiltered, rightEdgePointsFiltered; // 滤波后的左右边界点
            filterXCoord(leftEdgePoints, leftEdgePointsFiltered, 19); // 对原始的左边点进行强滤波
            filterXCoord(rightEdgePoints, rightEdgePointsFiltered, 19); // 对原始的右边点进行强滤波
            size_t minDopNum = std::min<size_t>(leftEdgePointsFiltered.size(), rightEdgePointsFiltered.size()); // 获取这两个容器的元素数量最小值
            std::vector<cv::Point> midPointsFiltered; // 用于存储滤波后的中点坐标
            for (size_t i=0; i<minDopNum; i++) {
                midPointsFiltered.push_back(
                    calMidPoint(leftEdgePointsFiltered.at(i),rightEdgePointsFiltered.at(i)) // 加入这个中点
                );
            }
            double py = calAverXCoordFromRowToRow(resizedFrame, midPointsFiltered, 30, 50); // 计算X坐标平均值
            // if (py > pyPID.targetVal) { // 如果中线偏右
            //     xlPID.measuredVal = xlPID.targetVal - 0.09;
            //     pyPID.measuredVal = pyPID.targetVal;
            // } else { // 如果中线偏左
            //     xlPID.measuredVal = xlPID.targetVal + 0.09;
            //     pyPID.measuredVal = pyPID.targetVal;
            // }
            if (py < pyPID.targetVal) { // 如果中线偏左
                xlPID.measuredVal = xlPID.targetVal + 0.43;
                pyPID.measuredVal = pyPID.targetVal;
            }
        }

        /**************************舵机硬件驱动********************************/
        angleOffset = xlPID.pidCalculate() + pyPID.pidCalculate(); // 将两个PID输出相加，得到舵机的转角偏移值
        angleOffset = myClamp(angleOffset, -SERVO_OFFSET_ANGLE_MAX, +SERVO_OFFSET_ANGLE_MAX); // 限制舵机转角偏移值的范围
        finalAngle = SERVO_ANGLE_MID + angleOffset; // 在中值 SERVO_ANGLE_MID 的基础之上
        servoPWM.setDutyCycle( angle2cnt(finalAngle, SERVO_CNT_MAX) ); // 将PID输出加载到舵机

        /************************终端打印信息**************************/
        clearScreen_ESCAPE; // 清空终端
        std::cout
            << "测量距离：" << od << "(mm)" << "\n"
            << "过近计数：" << "[" << jlgjCnt << "/" <<jlgjCntMAX << "]" << "\n"
            << "避障开启：" << ((isOnAvds) ? "是" : "否") << "\n"
            << "--------------------------------" << "\n"
            << "帧率：" << fpsCnt.fpsStr << "\n"
            << "阈值：" << GRAY2BIN_THRESH << "\n"
            << "偏斜：" << xlPID.measuredVal << "\n"
            << "偏移：" << pyPID.measuredVal << "\n"
            << "转向：" << ( ((xlPID.measuredVal-xlPID.targetVal) < -0.2) ? "向右" : 
                          ( ((xlPID.measuredVal-xlPID.targetVal) > +0.2) ? "向左" : "直走")) << "\n"
            << "角度：" << finalAngle << " (" << angle2cnt(finalAngle, SERVO_CNT_MAX) << ")" << "\n"
            << "--------------------------------" << "\n"
            << FG_RED+BOLD+UNDERLINE << "按空格键启动/停止电机" << STYLE_RST << "\n"
            << "电机启动：" << ((isMotorEnabled) ? "是" : "否") << std::endl;
    }

    cam.release(); // 释放相机资源
    std::cout << "相机资源已释放" << std::endl; // 添加日志来跟踪资源释放

    servoPWM.setDutyCycle( angle2cnt(SERVO_ANGLE_MID, SERVO_CNT_MAX) );
    usleep(ms2us(500));

    return 0;
}