// 一些辅助程序
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
#include <numeric>      // 提供数值操作的算法，如累加、累积等。
#include "test.hpp"

// 使用自动阈值，将彩色图像转为指定像素的二值图像，返回值为转换为二值化图像时的阈值
double bgr2bin(const cv::Mat& bgrImg, cv::Mat& resizedImg, cv::Mat& grayImg, cv::Mat& binImg,
               const cv::Size& imgSize) {
    cv::resize(bgrImg, resizedImg, imgSize);
    cv::cvtColor(resizedImg, grayImg, cv::COLOR_BGR2GRAY); // 将原始图像转换为灰度图
    return cv::threshold(grayImg, binImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}

// 指定阈值，将彩色图像转为指定像素的二值图像
double bgr2bin(const cv::Mat& bgrImg, cv::Mat& resizedImg, cv::Mat& grayImg, cv::Mat& binImg,
             const cv::Size& imgSize, double thresh, double maxval) {
    cv::resize(bgrImg, resizedImg, imgSize);
    cv::cvtColor(resizedImg, grayImg, cv::COLOR_BGR2GRAY); // 将原始图像转换为灰度图
    cv::threshold(grayImg, binImg, thresh, maxval, cv::THRESH_BINARY);
    return thresh;
}

/*判断二值化图像中的某个矩形区域是否都为color颜色，是的话返回true，否的话返回false，如果传入的矩形范围超过图像尺寸也会返回false*/
bool isBinImgRectRoiAllColor(const cv::Mat& binImg, const cv::Rect& rect, uint8 color) {
    if (rect.x < 0 || rect.y < 0 || rect.width <= 0 || rect.height <= 0) return false; // 检查数据是否合法
    if (rect.x + rect.width > binImg.cols || rect.y + rect.height > binImg.rows) return false; // 检查矩形区域是否在图像范围内
    cv::Mat imgRoi = binImg(rect); // 提取矩形区域
    for (int i = 0; i < imgRoi.rows; ++i) // 遍历矩形区域中的所有像素
        for (int j = 0; j < imgRoi.cols; ++j)
            if (imgRoi.at<uint8>(i,j) != color) return false; // 如果发现非黑色像素，返回false
    return true; // 所有像素都是黑色，返回true
}

/*这是一个在黑色大块中寻找白色小块的函数，会将得到的白色小块的中心坐标点存入到vector容器centers中*/
void getWhiteCentersOfSmallRegions(const cv::Mat& binImg, double minArea, std::vector<cv::Point>& centers) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // 查找轮廓
    for (auto& contour : contours) { // 遍历所有轮廓
        double area = cv::contourArea(contour); // 计算轮廓的面积
        if (area < minArea) {  // 如果面积小于最小面积阈值，则计算中心点
            cv::Moments m = cv::moments(contour);
            int cx = static_cast<int>(m.m10 / m.m00);
            int cy = static_cast<int>(m.m01 / m.m00);
            centers.push_back(cv::Point(cx, cy));
        }
    }
}

/*这是一个在白色大块中寻找黑色小块的函数，会将得到的黑色小块的中心坐标点存入到vector容器centers中*/
void getBlackCentersOfSmallRegions(const cv::Mat& binImg, double minArea, std::vector<cv::Point>& centers) {
    cv::Mat binInvImg; // 原二值图像 > 二值图像翻转图像
    cv::bitwise_not(binImg, binInvImg); // 二值图像翻转
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binInvImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // 查找轮廓
    for (auto& contour : contours) { // 遍历所有轮廓
        double area = cv::contourArea(contour); // 计算轮廓的面积
        if (area < minArea) {  // 如果面积小于最小面积阈值，则计算中心点
            cv::Moments m = cv::moments(contour);
            int cx = static_cast<int>(m.m10 / m.m00);
            int cy = static_cast<int>(m.m01 / m.m00);
            centers.push_back(cv::Point(cx, cy));
        }
    }
}

/*洪水填充二值图像某一区域的颜色，就像Windows画图软件的倒墨水*/
void fillBlobRecursive(cv::Mat& binImg, cv::Point pt, uint8 newColor) {
    if (pt.x < 0 || pt.x >= binImg.cols || pt.y < 0 || pt.y >= binImg.rows) return; // 检查坐标是否超出图像边界
    uint8 currentColor = binImg.at<uint8>(pt); // 获取当前像素点的灰度值
    if (currentColor == newColor) return; // 如果当前像素点已经是新颜色，或者不是旧颜色，则返回
    binImg.at<uint8>(pt) = newColor; // 将当前像素点更换为指定的新颜色

    /*递归调用，填充上下左右四个方向的像素点*/
    fillBlobRecursive(binImg, cv::Point(pt.x + 1, pt.y), newColor);
    fillBlobRecursive(binImg, cv::Point(pt.x - 1, pt.y), newColor);
    fillBlobRecursive(binImg, cv::Point(pt.x, pt.y + 1), newColor);
    fillBlobRecursive(binImg, cv::Point(pt.x, pt.y - 1), newColor);
}

/*洪水填充彩色图某一区域的颜色，就像Windows画图软件的倒墨水*/
// 注：对于jpg格式，其压缩保存有时候会使一部分像素的RGB通道变化，导致洪水填充并不能取得较好的预期效果，这个彩色填充少点使用。
void fillBlobRecursive(cv::Mat& bgrImg, cv::Point pt, cv::Vec3b newColor, cv::Vec3b oldColor) {
    if (pt.x < 0 || pt.x >= bgrImg.cols || pt.y < 0 || pt.y >= bgrImg.rows) return;
    cv::Vec3b currentColor = bgrImg.at<cv::Vec3b>(pt);
    if (currentColor == newColor || currentColor != oldColor) return;
    bgrImg.at<cv::Vec3b>(pt) = newColor;
    /*递归调用，填充上下左右四个方向的像素点*/
    fillBlobRecursive(bgrImg, cv::Point(pt.x + 1, pt.y), newColor, oldColor);
    fillBlobRecursive(bgrImg, cv::Point(pt.x - 1, pt.y), newColor, oldColor);
    fillBlobRecursive(bgrImg, cv::Point(pt.x, pt.y + 1), newColor, oldColor);
    fillBlobRecursive(bgrImg, cv::Point(pt.x, pt.y - 1), newColor, oldColor);
}

/**
 * @brief 传入一张二值化图像，并将左右的边界点压入到两个容器中
 * @note  搜寻方向是：从左到右、从上到下（都是0索引起始）
 * @param binImg 二值化图像
 * @param LeftEdgePoints 收集左边界点的容器引用
 * @param rightEdgePoints 收集右边界点的容器引用
 */
// 使用哪一种方法来查找边线
#define use_findEdgePix  5 // 使用第几种方法
void findEdgePix(const cv::Mat binImg, std::vector<cv::Point>& leftEdgePoints, std::vector<cv::Point>& rightEdgePoints) {
#if use_findEdgePix == 1
    // 1、简单的边线识别：从左到右依此寻找 黑转白 和 白转黑 的点，分别用作左右边界点===============================================
    bool findLeftEdge = false, findRightEdge = false;
    for (int r = 0; r < binImg.rows; r++) {
       findLeftEdge = true; findRightEdge = false;
       for (int c = 0; c < binImg.cols; c++) {
           uint8 pix = binImg.at<uint8>(r, c);
           if (findLeftEdge && pix == BIN_WHITE) {
               leftEdgePoints.push_back(cv::Point(c, r));
               findLeftEdge = false;
               findRightEdge = true;
           }
           else if (findRightEdge && pix == BIN_BLACK) {
               rightEdgePoints.push_back(cv::Point(c, r));
               findLeftEdge = true;
               findLeftEdge = false;
               break;
           }
       }
    }

#elif use_findEdgePix == 2
    // 2、带有怀疑的边线识别：比如在同一行中，由黑转白，要连续识别到多个白色像素，才能认定为左边线===============================================
    bool findLeftEdge = false, findRightEdge = false; // 是否在查找左边线，是否在查找右边线
    bool leftEdge_huaiyi = false, rightEdge_huaiyi = false; // 怀疑是否为边线了
    uint8 leftEdge_huaiyi_cnt = 0, rightEdge_huaiyi_cnt = 0; // 怀疑计数
    constexpr auto leftEdge_huaiyi_cnt_MAX = 10, rightEdge_huaiyi_cnt_MAX = 5; // 要连续怀疑了这么多次·，才会被认定为边线
    cv::Point leftEdge_huaiyi_mark = cv::Point(0, 0), rightEdge_huaiyi_mark = cv::Point(0, 0); // 怀疑是否为边线的标记点
    for (int r = 0; r < binImg.rows; r++) {
        /*在每一行起始时*/
        findLeftEdge = true; findRightEdge = false;
        leftEdge_huaiyi = false; rightEdge_huaiyi = false;
        leftEdge_huaiyi_cnt = 0; rightEdge_huaiyi_cnt = 0;
        leftEdge_huaiyi_mark = cv::Point(0, 0); rightEdge_huaiyi_mark = cv::Point(0, 0);
        for (int c = 0; c < binImg.cols; c++) {
            uint8 pix = binImg.at<uint8>(r, c);
            if (findLeftEdge) {
                /*没有怀疑时，发现白色块了，置怀疑，并记录当前白色块坐标*/
                if (!leftEdge_huaiyi && pix == BIN_WHITE) {
                    leftEdge_huaiyi = true; // 置怀疑
                    leftEdge_huaiyi_cnt++; // 怀疑度+1
                    leftEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
                }
                /*还没怀疑到极限，发现了白色块，怀疑加深*/
                else if (leftEdge_huaiyi && leftEdge_huaiyi_cnt < leftEdge_huaiyi_cnt_MAX && pix == BIN_WHITE) {
                    leftEdge_huaiyi_cnt++; // 怀疑度+1
                }
                /*还没怀疑到极限，发现黑色块了，消除怀疑，怀疑度清零*/
                else if (leftEdge_huaiyi && leftEdge_huaiyi_cnt < leftEdge_huaiyi_cnt_MAX && pix == BIN_BLACK) {
                    leftEdge_huaiyi = false; // 消除怀疑
                    leftEdge_huaiyi_cnt = 0; // 怀疑度清零
                }
                /*怀疑到极限了，确定无误是左边界了*/
                else if (leftEdge_huaiyi && leftEdge_huaiyi_cnt >= leftEdge_huaiyi_cnt_MAX) {
                    leftEdgePoints.push_back(leftEdge_huaiyi_mark); // 记录这个左边界点
                    findLeftEdge = false; findRightEdge = true; // 开始搜寻右边界
                }
            }
            else if (findRightEdge) {
                /*如果一直没找到右边线*/
                if (c == binImg.cols - 1) {
                    // 还没有怀疑，则直接把这行最后一个点加进去
                    if (!rightEdge_huaiyi) rightEdgePoints.push_back(cv::Point(c, r));
                    // 有怀疑了，但还没有怀疑到极限，则把怀疑的点加进去
                    else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt <= rightEdge_huaiyi_cnt_MAX) {
                        rightEdgePoints.push_back(rightEdge_huaiyi_mark);
                    }
                }
                /*没有怀疑时，发现黑色块了，置怀疑，并记录当前黑色块坐标*/
                else if (!rightEdge_huaiyi && pix == BIN_BLACK) {
                    rightEdge_huaiyi = true; // 置怀疑
                    rightEdge_huaiyi_cnt++; // 怀疑度+1
                    rightEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
                }
                /*还没怀疑到极限，发现了黑色块，怀疑加深*/
                else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt < rightEdge_huaiyi_cnt_MAX && pix == BIN_BLACK) {
                    rightEdge_huaiyi_cnt++; // 怀疑度+1
                }
                /*还没怀疑到极限，发现白色块了，消除怀疑，怀疑度清零*/
                else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt < rightEdge_huaiyi_cnt_MAX && pix == BIN_WHITE) {
                    rightEdge_huaiyi = false; // 消除怀疑
                    rightEdge_huaiyi_cnt = 0; // 怀疑度清零
                }
                /*怀疑到极限了，确认无误是右边界了*/
                else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt >= rightEdge_huaiyi_cnt_MAX) {
                    rightEdgePoints.push_back(rightEdge_huaiyi_mark); // 记录这个右边界点
                    findLeftEdge = true; findRightEdge = false; // 开始搜寻左边界
                    break;
                }
            }
        }
    }

#elif use_findEdgePix == 3
    // 3、带有怀疑，从中点往两边找边线，要连续识别到多个黑色像素，才能认定为边线===============================================
    bool huaiyi = false; // 是否怀疑
    uint8 huaiyi_cnt = 0; // 怀疑计数
    constexpr int huaiyi_cnt_MAX = 5; // 怀疑计数最大值
    cv::Point leftEdge_huaiyi_mark = cv::Point(0, 0), rightEdge_huaiyi_mark = cv::Point(0, 0); // 怀疑是否为边线的标记点
    for (int r=0; r<binImg.rows; r++) {
        /*每一行起始*/
        leftEdge_huaiyi_mark = cv::Point(0, 0); rightEdge_huaiyi_mark = cv::Point(0, 0);
        /*搜寻左边界*/
        huaiyi = false;
        huaiyi_cnt = 0;
        for (int c=binImg.cols/2; c>=0; c--) {
            /*从中点往左边找*/
            uint8 pix = binImg.at<uint8>(r, c);
            /*如果一直没找到左边线*/
            if(c == 0) {
                // 还没有怀疑，则直接把搜寻的最后一个点加进去
                if(!huaiyi) leftEdgePoints.push_back(cv::Point(c, r));
                // 有怀疑了，但还没有怀疑到极限，则把怀疑的点加进去
                else if (huaiyi && huaiyi_cnt <= huaiyi_cnt_MAX) {
                    leftEdgePoints.push_back(leftEdge_huaiyi_mark);
                }
            }
            /*没有怀疑时，发现黑色块了，置怀疑，并记录当前黑色块坐标*/
            else if(!huaiyi && pix == BIN_BLACK) {
                huaiyi = true; // 置怀疑
                huaiyi_cnt++; // 怀疑度+1
                leftEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
            }
            /*还没怀疑到极限，发现了黑色块，怀疑加深*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_BLACK) {
                huaiyi_cnt++; // 怀疑度+1
            }
            /*还没怀疑到极限，发现白色块了，消除怀疑，怀疑度清零*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_WHITE) {
                huaiyi = false; // 消除怀疑
                huaiyi_cnt = 0; // 怀疑度清零
            }
            /*怀疑到极限了，确认无误是左边界了*/
            else if(huaiyi && huaiyi_cnt >= huaiyi_cnt_MAX) {
                leftEdgePoints.push_back(leftEdge_huaiyi_mark); // 记录这个左边界点
                huaiyi = false; // 不怀疑了，确定了
                huaiyi_cnt = 0; // 怀疑度清零
                break; // 退出循环，开始搜寻右边界
            }
        }
        /*搜寻右边界*/
        huaiyi = false;
        huaiyi_cnt = 0;
        for (int c=binImg.cols/2; c<binImg.cols; c++) {
            /*从中点往右边找*/
            uint8 pix = binImg.at<uint8>(r, c);
            /*如果一直没找到右边线*/
            if(c == binImg.cols - 1) {
                // 还没有怀疑，则直接把搜寻的最后一个点加进去
                if(!huaiyi) rightEdgePoints.push_back(cv::Point(c, r));
                // 有怀疑了，但还没有怀疑到极限，则把怀疑的点加进去
                else if (huaiyi && huaiyi_cnt <= huaiyi_cnt_MAX) {
                    rightEdgePoints.push_back(rightEdge_huaiyi_mark);
                }
            }
            /*没有怀疑时，发现黑色块了，置怀疑，并记录当前黑色块坐标*/
            else if(!huaiyi && pix == BIN_BLACK) {
                huaiyi = true; // 置怀疑
                huaiyi_cnt++; // 怀疑度+1
                rightEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
            }
            /*还没怀疑到极限，发现了黑色块，怀疑加深*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_BLACK) {
                huaiyi_cnt++; // 怀疑度+1
            }
            /*还没怀疑到极限，发现白色块了，消除怀疑，怀疑度清零*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_WHITE) {
                huaiyi = false; // 消除怀疑
                huaiyi_cnt = 0; // 怀疑度清零
            }
            /*怀疑到极限了，确认无误是右边界了*/
            else if(huaiyi && huaiyi_cnt >= huaiyi_cnt_MAX) {
                rightEdgePoints.push_back(rightEdge_huaiyi_mark); // 记录这个右边界点
                huaiyi = false; // 不怀疑了，确定了
                huaiyi_cnt = 0; // 怀疑度清零
                break; // 退出循环
            }
        }
    }
#elif use_findEdgePix == 4
    // 4、先从底部开始，以第2种方法找到一组边界点，如果没找到，则继续往上找，
    //    在找到一组边界点后，就开始往上找边线，每轮到上一行，只在在前一个边界点横坐标附近找边界点
    bool hasFoundBasis = false; // 是否已经找到基础识别点
    constexpr int FIND_ZONE = 15; // 查找区域
    bool findLeftEdge = false, findRightEdge = false; // 是否在查找左边线，是否在查找右边线
    bool leftEdge_huaiyi = false, rightEdge_huaiyi = false; // 怀疑是否为边线了
    uint8 leftEdge_huaiyi_cnt = 0, rightEdge_huaiyi_cnt = 0; // 怀疑计数
    constexpr auto leftEdge_huaiyi_cnt_MAX = 10, rightEdge_huaiyi_cnt_MAX = 20; // 要连续怀疑了这么多次·，才会被认定为边线
    cv::Point leftEdge_huaiyi_mark = cv::Point(0, 0), rightEdge_huaiyi_mark = cv::Point(0, 0); // 怀疑是否为边线的标记点
    for (int r=binImg.rows-1; r>=0; r--) {
        /*从最低行开始，先查找一个基础识别点，再往上推进（用的是第2种查找边线的方法）*/
        if (!hasFoundBasis) { // 如果还没有找到基础识别点
            /*在每一行起始时*/
            findLeftEdge = true; findRightEdge = false;
            leftEdge_huaiyi = false; rightEdge_huaiyi = false;
            leftEdge_huaiyi_cnt = 0; rightEdge_huaiyi_cnt = 0;
            leftEdge_huaiyi_mark = cv::Point(0, 0); rightEdge_huaiyi_mark = cv::Point(0, 0);
            for (int c = 0; c < binImg.cols; c++) {
                uint8 pix = binImg.at<uint8>(r, c);
                if (findLeftEdge) {
                    /*没有怀疑时，发现白色块了，置怀疑，并记录当前白色块坐标*/
                    if (!leftEdge_huaiyi && pix == BIN_WHITE) {
                        leftEdge_huaiyi = true; // 置怀疑
                        leftEdge_huaiyi_cnt++; // 怀疑度+1
                        leftEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
                    }
                    /*还没怀疑到极限，发现了白色块，怀疑加深*/
                    else if (leftEdge_huaiyi && leftEdge_huaiyi_cnt < leftEdge_huaiyi_cnt_MAX && pix == BIN_WHITE) {
                        leftEdge_huaiyi_cnt++; // 怀疑度+1
                    }
                    /*还没怀疑到极限，发现黑色块了，消除怀疑，怀疑度清零*/
                    else if (leftEdge_huaiyi && leftEdge_huaiyi_cnt < leftEdge_huaiyi_cnt_MAX && pix == BIN_BLACK) {
                        leftEdge_huaiyi = false; // 消除怀疑
                        leftEdge_huaiyi_cnt = 0; // 怀疑度清零
                    }
                    /*怀疑到极限了，确定无误是左边界了*/
                    else if (leftEdge_huaiyi && leftEdge_huaiyi_cnt >= leftEdge_huaiyi_cnt_MAX) {
                        leftEdgePoints.push_back(leftEdge_huaiyi_mark); // 记录这个左边界点
                        findLeftEdge = false; findRightEdge = true; // 开始搜寻右边界
                    }
                }
                else if (findRightEdge) {
                    /*如果一直没找到右边线*/
                    if (c == binImg.cols - 1) {
                        // 还没有怀疑，则直接把这行最后一个点加进去
                        if (!rightEdge_huaiyi) rightEdgePoints.push_back(cv::Point(c, r));
                        // 有怀疑了，但还没有怀疑到极限，则把怀疑的点加进去
                        else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt <= rightEdge_huaiyi_cnt_MAX) {
                            rightEdgePoints.push_back(rightEdge_huaiyi_mark);
                        }
                    }
                    /*没有怀疑时，发现黑色块了，置怀疑，并记录当前黑色块坐标*/
                    else if (!rightEdge_huaiyi && pix == BIN_BLACK) {
                        rightEdge_huaiyi = true; // 置怀疑
                        rightEdge_huaiyi_cnt++; // 怀疑度+1
                        rightEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
                    }
                    /*还没怀疑到极限，发现了黑色块，怀疑加深*/
                    else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt < rightEdge_huaiyi_cnt_MAX && pix == BIN_BLACK) {
                        rightEdge_huaiyi_cnt++; // 怀疑度+1
                    }
                    /*还没怀疑到极限，发现白色块了，消除怀疑，怀疑度清零*/
                    else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt < rightEdge_huaiyi_cnt_MAX && pix == BIN_WHITE) {
                        rightEdge_huaiyi = false; // 消除怀疑
                        rightEdge_huaiyi_cnt = 0; // 怀疑度清零
                    }
                    /*怀疑到极限了，确认无误是右边界了*/
                    else if (rightEdge_huaiyi && rightEdge_huaiyi_cnt >= rightEdge_huaiyi_cnt_MAX) {
                        rightEdgePoints.push_back(rightEdge_huaiyi_mark); // 记录这个右边界点
                        findLeftEdge = true; findRightEdge = false; // 开始搜寻左边界
                        break;
                    }
                }
            }
            if (leftEdgePoints.empty() || rightEdgePoints.empty()) { // 如果有一边还没有找到基础识别点（边界点）
                hasFoundBasis = false; // 保持为false，表示还没有找到基础识别点
                leftEdgePoints.erase(leftEdgePoints.begin(), leftEdgePoints.end()); // 清空左边线点
                rightEdgePoints.erase(rightEdgePoints.begin(), rightEdgePoints.end()); // 清空右边线点
                // 清空是为了能保持寻找到的点能纵坐标（行号）对齐
            }
            else { // 如果找到了边线
                hasFoundBasis = true; // 置为true，表示已经找到基础识别点了
            }
        }
        else { // 好的，已经找到基础识别点了，开始不断向上查找
            cv::Point leftLastMark(leftEdgePoints.back().x, leftEdgePoints.back().y); // 左边线最后一个点
            for (int cl=leftLastMark.x+FIND_ZONE; cl>=leftLastMark.x-FIND_ZONE; cl--) { // 左边线，从右往左找边界
                if (isBinImgRectRoiAllColor(
                        binImg,
                        cv::Rect(
                            (leftLastMark.x-FIND_ZONE<0) ? 0 : leftLastMark.x-FIND_ZONE, // 这是起始横坐标值，如果小于0的话，则直接为0，否则为原数
                            r, // 这是起始纵坐标值
                            (leftLastMark.x+FIND_ZONE>binImg.cols) ? binImg.cols-(leftLastMark.x-FIND_ZONE) : FIND_ZONE*2+1, // 这是宽度值，如果大于图像宽度的话，则直接为图像宽度减去起始横坐标值，否则为FIND_ZONE*2+1
                            1 // 这是高度值
                        ),
                        BIN_BLACK)// 如果这个区域都是黑色的
                ) {
                    leftEdgePoints.push_back(cv::Point(leftLastMark.x, r)); // 保持上一次记录的横坐标值
                    break; // 结束循环
                }
                else if(isBinImgRectRoiAllColor(
                        binImg,
                        cv::Rect(
                            (leftLastMark.x-FIND_ZONE<0) ? 0 : leftLastMark.x-FIND_ZONE, // 这是起始横坐标值，如果小于0的话，则直接为0，否则为原数
                            r, // 这是起始纵坐标值
                            (leftLastMark.x+FIND_ZONE>binImg.cols) ? binImg.cols-(leftLastMark.x-FIND_ZONE) : FIND_ZONE*2+1, // 这是宽度值，如果大于图像宽度的话，则直接为图像宽度减去起始横坐标值，否则为FIND_ZONE*2+1
                            1 // 这是高度值
                        ),
                        BIN_WHITE) // 如果这个区域都是白色的
                ) {
                    leftEdgePoints.push_back(cv::Point(leftLastMark.x, r)); // 保持上一次记录的横坐标值
                    break; // 结束循环
                }
                else if (cl >= binImg.cols) continue; // 越界了，直接下一次循环
                else if (cl < 0) {
                    leftEdgePoints.push_back(cv::Point(leftLastMark.x, r)); // 保持上一次记录的横坐标值
                    break; // 结束循环
                }
                else if (binImg.at<uint8>(r,cl) == BIN_BLACK) {
                    leftEdgePoints.push_back(cv::Point(cl, r)); // 记录这个边界点
                    break; // 结束循环
                }
            }
            cv::Point rightLastMark(rightEdgePoints.back().x, rightEdgePoints.back().y); // 右边线最后一个点
            for (int cr=rightLastMark.x-FIND_ZONE; cr<=rightLastMark.x+FIND_ZONE; cr++) { // 右边线，从左往右找边界
                if (isBinImgRectRoiAllColor(
                        binImg,
                        cv::Rect(
                            (rightLastMark.x-FIND_ZONE<0) ? 0 : rightLastMark.x-FIND_ZONE, // 这是起始横坐标值，如果小于0的话，则直接为0，否则为原数
                            r, // 这是起始纵坐标值
                            (rightLastMark.x+FIND_ZONE>binImg.cols)?binImg.cols-(rightLastMark.x-FIND_ZONE):FIND_ZONE*2+1, // 这是宽度值，如果大于图像宽度的话，则直接为图像宽度减去起始横坐标值，否则为FIND_ZONE*2+1
                            1 // 这是高度值
                        ),
                        BIN_BLACK) // 如果这个区域都是黑色的
                ) {
                    rightEdgePoints.push_back(cv::Point(rightLastMark.x, r)); // 保持上一次记录的横坐标值
                    break; // 结束循环
                }
                else if(isBinImgRectRoiAllColor(
                        binImg,
                        cv::Rect(
                            (rightLastMark.x-FIND_ZONE<0) ? 0 : rightLastMark.x-FIND_ZONE, // 这是起始横坐标值，如果小于0的话，则直接为0，否则为原数
                            r, // 这是起始纵坐标值
                            (rightLastMark.x+FIND_ZONE>binImg.cols)?binImg.cols-(rightLastMark.x-FIND_ZONE):FIND_ZONE*2+1, // 这是宽度值，如果大于图像宽度的话，则直接为图像宽度减去起始横坐标值，否则为FIND_ZONE*2+1
                            1 // 这是高度值
                        ),
                        BIN_WHITE) // 如果这个区域都是白色的
                ) {
                    rightEdgePoints.push_back(cv::Point(rightLastMark.x, r)); // 保持上一次记录的横坐标值
                    break; // 结束循环
                }
                else if (cr < 0) continue; // 越界了，直接下一次循环
                else if (cr >= binImg.cols) {
                    rightEdgePoints.push_back(cv::Point(rightLastMark.x, r)); // 保持上一次记录的横坐标值
                    break; // 结束循环
                }
                else if (binImg.at<uint8>(r, cr) == BIN_BLACK) {
                    rightEdgePoints.push_back(cv::Point(cr, r)); // 记录这个边界点
                    break; // 结束循环
                }
            }
        }
    }
#elif use_findEdgePix == 5
    // 5、从底部向上，从中点往两边找边线，在找到一组边线后，以找到的边线计算出的中点作为新的中点，
    //    轮到上一行，就以这个新的中点的x坐标在该行往两边搜线，要连续识别到多个黑色像素，才能认定为边线
    bool huaiyi = false; // 是否怀疑
    uint8 huaiyi_cnt = 0; // 怀疑计数
    constexpr int huaiyi_cnt_MAX = 7; // 怀疑计数最大值
    cv::Point midPointMark = cv::Point(binImg.cols/2, binImg.rows-1); // 中点标记，初始为图像底部中点
    cv::Point leftEdge_huaiyi_mark = cv::Point(0, 0), rightEdge_huaiyi_mark = cv::Point(0, 0); // 怀疑是否为边线的标记点
    for (int r=binImg.rows-1; r>=0; r--) { // 从底部向上
        /*每一行起始*/
        leftEdge_huaiyi_mark = cv::Point(0, 0); rightEdge_huaiyi_mark = cv::Point(0, 0);
        /*搜寻左边界*/
        huaiyi = false;
        huaiyi_cnt = 0;
        for (int c=midPointMark.x; c>=0; c--) {
            /*从中点往左边找*/
            uint8 pix = binImg.at<uint8>(r, c);
            /*如果一直没找到左边线*/
            if(c == 0) {
                // 还没有怀疑，则直接把搜寻的最后一个点加进去
                if(!huaiyi) leftEdgePoints.push_back(cv::Point(c, r));
                // 有怀疑了，但还没有怀疑到极限，则把怀疑的点加进去
                else if (huaiyi && huaiyi_cnt <= huaiyi_cnt_MAX) {
                    leftEdgePoints.push_back(leftEdge_huaiyi_mark);
                }
            }
            /*没有怀疑时，发现黑色块了，置怀疑，并记录当前黑色块坐标*/
            else if(!huaiyi && pix == BIN_BLACK) {
                huaiyi = true; // 置怀疑
                huaiyi_cnt++; // 怀疑度+1
                leftEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
            }
            /*还没怀疑到极限，发现了黑色块，怀疑加深*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_BLACK) {
                huaiyi_cnt++; // 怀疑度+1
            }
            /*还没怀疑到极限，发现白色块了，消除怀疑，怀疑度清零*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_WHITE) {
                huaiyi = false; // 消除怀疑
                huaiyi_cnt = 0; // 怀疑度清零
            }
            /*怀疑到极限了，确认无误是左边界了*/
            else if(huaiyi && huaiyi_cnt >= huaiyi_cnt_MAX) {
                leftEdgePoints.push_back(leftEdge_huaiyi_mark); // 记录这个左边界点
                huaiyi = false; // 不怀疑了，确定了
                huaiyi_cnt = 0; // 怀疑度清零
                break; // 退出循环，开始搜寻右边界
            }
        }
        /*搜寻右边界*/
        huaiyi = false;
        huaiyi_cnt = 0;
        for (int c=midPointMark.x; c<binImg.cols; c++) {
            /*从中点往右边找*/
            uint8 pix = binImg.at<uint8>(r, c);
            /*如果一直没找到右边线*/
            if(c == binImg.cols - 1) {
                // 还没有怀疑，则直接把搜寻的最后一个点加进去
                if(!huaiyi) rightEdgePoints.push_back(cv::Point(c, r));
                // 有怀疑了，但还没有怀疑到极限，则把怀疑的点加进去
                else if (huaiyi && huaiyi_cnt <= huaiyi_cnt_MAX) {
                    rightEdgePoints.push_back(rightEdge_huaiyi_mark);
                }
            }
            /*没有怀疑时，发现黑色块了，置怀疑，并记录当前黑色块坐标*/
            else if(!huaiyi && pix == BIN_BLACK) {
                huaiyi = true; // 置怀疑
                huaiyi_cnt++; // 怀疑度+1
                rightEdge_huaiyi_mark = cv::Point(c, r); // 记录当前怀疑坐标
            }
            /*还没怀疑到极限，发现了黑色块，怀疑加深*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_BLACK) {
                huaiyi_cnt++; // 怀疑度+1
            }
            /*还没怀疑到极限，发现白色块了，消除怀疑，怀疑度清零*/
            else if(huaiyi && huaiyi_cnt < huaiyi_cnt_MAX && pix == BIN_WHITE) {
                huaiyi = false; // 消除怀疑
                huaiyi_cnt = 0; // 怀疑度清零
            }
            /*怀疑到极限了，确认无误是右边界了*/
            else if(huaiyi && huaiyi_cnt >= huaiyi_cnt_MAX) {
                rightEdgePoints.push_back(rightEdge_huaiyi_mark); // 记录这个右边界点
                huaiyi = false; // 不怀疑了，确定了
                huaiyi_cnt = 0; // 怀疑度清零
                break; // 退出循环
            }
        }
        if (leftEdgePoints.empty() || rightEdgePoints.empty()) continue;
        else midPointMark = calMidPoint(leftEdgePoints.back(), rightEdgePoints.back()); // 计算新的中点
    }
#endif
}

/*在图像上的指定行画一条直线*/
void drawLineOnRow(cv::Mat& img, int rowIndex, cv::Scalar color, int thickness) {
    cv::line(img, cv::Point(0, rowIndex), cv::Point(img.cols - 1, rowIndex), color, thickness);
}

// 计算两个坐标之间的中点坐标
cv::Point calMidPoint(const cv::Point pt1, const cv::Point& pt2) {
    return cv::Point((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2);
}

/*计算一个double类型容器的平均值*/
// 计算过程
// 8 2 7
// (0 * 0 + 8) / 1 = 8
// (8 * 1 + 2) / 2 = 5
// (5 * 2 + 7) / 3 ≈ 5.67
// 变化后
// 0 / 1 * 0 + 8 / 1 = 8
// 8 / 2 * 1 + 2 / 2 = 5
// 5 / 3 * 2 + 7 / 3 ≈ 5.67
// 这样可以防止加的总和过大，导致数据超出类型表示范围。
double myCalAverage(const std::vector<double> vdata) {
    if ( vdata.empty() ) return 0.0f; // 返回默认构造的值，对于浮点数是 0.0，对于整数也是 0
    else {
        double cnt=0, numMark=0;
        for (auto& v : vdata) {
			numMark = numMark / (cnt + 1) * cnt + v / (cnt + 1); // 先除后乘是为了防止数据溢出
            cnt += 1;
        }
        return numMark;
    }
}

template <typename T>
T calAverage(const std::vector<T> vdata) {
    if ( vdata.empty() ) return T(); // 如果vector容器是空的，返回0
    else return ( std::accumulate(vdata.begin(), vdata.end(), T()) / (T)vdata.size() );
}

// Calculate average slope from row to row
// 计算两行之间中线点的平均斜率，以确定车向哪一边转弯
double calAverSlopeFromRowToRow(cv::Mat& img, std::vector<cv::Point> midPoint, int row_up, int row_down) {
    drawLineOnRow(img, row_up, cv::Scalar(BGR_ICEBLUE), 1); // 在第row_up行画一条厚度为1的直线
    drawLineOnRow(img, row_down, cv::Scalar(BGR_ICEBLUE), 1); // 在第row_down行画一条厚度为1的直线
    cv::Point pointMark(0, 0); // 用于记录前一次的坐标值，与当前坐标值进行计算得出斜率
    bool isFirst = true; // 用于在第一次进入的时候，只记录值而不计算
    std::vector<double> slope; // 用于记录斜率
    for (auto& pt : midPoint) { // 遍历循迹线的像素点
        if (pt.y > row_up && pt.y < row_down) { // 如果在row_up行到row_down行之间
            if (isFirst) { // 如果是第一次进来
                pointMark.x = pt.x; // 记录一下x坐标
                pointMark.y = pt.y; // 记录一下y坐标
                isFirst = false; // 之后就不是第一次进来了
            }
            else { // 如果不是第一次进来了
                slope.push_back((double)(pt.x - pointMark.x) / (double)(pt.y - pointMark.y)); // 计算出斜率，并加入容器
                pointMark.x = pt.x; // 更新记录x坐标
                pointMark.y = pt.y; // 更新记录y坐标
            }
            img.at<cv::Vec3b>(pt) = cv::Vec3b(BGR_BLACK); // 将算过的像素点变黑
        }
        else continue; // 如果不在行之间，则循环到下一个点（其实这条语句加不加都没关系，本来就要下一次循环了）
    }
    return calAverage(slope);
}

// Calculate average x coordinate from row to row
// 计算两行之间中线点的x坐标平均值，也作为车向哪一边转弯的一个影响因素
double calAverXCoordFromRowToRow(cv::Mat& img, std::vector<cv::Point> midPoint, int row_up, int row_down) {
	drawLineOnRow(img, row_up, cv::Scalar(BGR_ICEBLUE), 1); // 在第row_up行画一条厚度为1的直线
	drawLineOnRow(img, row_down, cv::Scalar(BGR_ICEBLUE), 1); // 在第row_down行画一条厚度为1的直线
	cv::line(img, cv::Point(img.cols/2,row_up), cv::Point(img.cols/2,row_down), cv::Scalar(BGR_ICEBLUE), 1); // 在中间画一条线
	std::vector<double> xCoord; // 用于记录x坐标
    for (auto& pt: midPoint) {
        if (pt.y > row_up && pt.y < row_down) {
			xCoord.push_back(pt.x); // 记录x坐标
        }
		else continue; // 如果不在行之间，则循环到下一个点（其实这条语句加不加都没关系，本来就要下一次循环了）
    }
    return calAverage(xCoord);
}

/*将角度映射到计数值，[0~180](d)->[0.5~2.5](ms)*/
int angle2cnt(double angle, int cntMAX) {
    // 以PWM整个周期计数cntMAX为参照，将[0,180]映射到对应的占空比计数值
    if(angle >= 0 && angle <= 180) // 如果角度在0~180之间，则映射到对应的计数值
        // return (500000.0 + angle * 11111.11111);
        // return ((double)cntMAX/20.0*0.5 + angle*((double)cntMAX/20.0*(2.5-0.5))/180.0);
        return ((double)cntMAX/40.0 + angle*((double)cntMAX/1800.0));
    else return ((double)cntMAX/40.0 + 90.0*((double)cntMAX/1800.0)); // 否则返回舵机中值对应的计数值
}


/*将速度的百分比数值映射至计数值*/
int speedPercent2cnt(double speedPercent, int cntMAX) {
    if(speedPercent >= 0 && speedPercent <= 100)
        return ((double)cntMAX / 100.0 * speedPercent);
    else return 0;
}

/*双精度浮点数的范围限制函数*/
double myClamp(const double val, const double low, const double high) {
    return (val < low) ? low : ( (val > high) ? high : val );
}

/*整型数的范围限制函数*/
int myClamp(const int val, const int low, const int high) {
    return (val < low) ? low : ( (val > high) ? high : val );
}

/**
 * @brief 将原始值映射到新的范围
 *
 * @param origValMin 原始值的最小值
 * @param origValMax 原始值的最大值
 * @param newValMin 新范围的最小值
 * @param newValMax 新范围的最大值
 * @param origVal 需要映射的原始值
 *
 * @return 映射后的新值
 */
double mapArea(double origValMin, double origValMax, double newValMin, double newValMax, double origVal) {
    return (origVal - origValMin) * (newValMax - newValMin) / (origValMax - origValMin) + newValMin;
}

/**
 * @brief 在二值图像四周绘制一个厚度为1的黑色矩形边框
 * @param binImg 二值图像，类型为CV_8UC1
 */
void drawBlackRectBorder(cv::Mat& binImg) {
    if (binImg.type() != CV_8UC1) { // 检查图像是否为二值图像
        std::cerr << "Image is not a binary image!" << std::endl;
        return;
    }
    // 获取图像的尺寸
    int rows = binImg.rows;
    int cols = binImg.cols;
    for (int j=0; j<cols; ++j) { // 在图像的顶部和底部画黑线
        binImg.at<uint8>(0,j) = 0;             // 顶部
        binImg.at<uint8>(rows-1,j) = 0;     // 底部
    }
    for (int i=0; i<rows; ++i) { // 在图像的左侧和右侧画黑线
        binImg.at<uint8>(i,0) = 0;             // 左侧
        binImg.at<uint8>(i,cols-1) = 0;     // 右侧
    }
}

/*反转二值图像中面积小于指定值的独立区域*/
// 方法一：检测二值图像中面积小于指定值的独立区域的中心坐标，然后使用洪水填充的方法将区域的颜色反转
void invertSmallRegion_1(cv::Mat& binImg, double minArea) {
    std::vector<cv::Point> centerWhite; // 用来存储面积小于某一阈值的白色区域的中心坐标值
    std::vector<cv::Point> centerBlack; // 用来存储面积小于某一阈值的黑色区域的中心坐标值
    getWhiteCentersOfSmallRegions(binImg, minArea, centerWhite); // 获取面积小于50的白色区域的中心坐标值
    getBlackCentersOfSmallRegions(binImg, minArea, centerBlack); // 获取面积小于50的黑色区域的中心坐标值
    for (auto& cenW : centerWhite)  fillBlobRecursive(binImg, cenW, BIN_BLACK); // 使用这些中心坐标值，洪水填充这些白色区域
    for (auto& cenB : centerBlack)  fillBlobRecursive(binImg, cenB, BIN_WHITE); // 使用这些中心坐标值，洪水填充这些黑色区域
}

/*反转二值图像中面积小于指定值的独立区域*/
// 方法二：直接使用OpenCV的findContours函数来查找轮廓，然后使用drawContours函数来反转颜色
void invertSmallRegion_2(cv::Mat& binImg, double minArea) {
    /*来自AI Copilot GPT-4o*/
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours.size(); i++) { // 遍历每个轮廓
        double area = cv::contourArea(contours[i]);
        if (area < minArea) { // 如果面积小于阈值，反转颜色
            cv::drawContours(binImg, contours, static_cast<int>(i), cv::Scalar(255-binImg.at<uint8>(contours[i][0])), cv::FILLED);
        }
    }
}

/**
 * @brief 对二值图像进行滤波处理，该函数会对传入的二值图像进行滤波处理，根据周围像素值的统计结果，修改中心像素的值。
 *
 * @param binImg 需要进行滤波处理的二值图像
 */
// 定义膨胀和腐蚀的阈值区间
#define threshold_max  255*5 //此参数可根据自己的需求调节
#define threshold_min  255*2 //此参数可根据自己的需求调节
void binImgFilter(cv::Mat& binImg) {
    uint32 num = 0;
    for (int i=1; i<RESIZED_HEIGHT-1; i++) {
        for (int j=1; j<RESIZED_WIDTH-1; j++) {
            //统计八个方向的像素值
            num = binImg.at<uint8>(i-1,j-1) + binImg.at<uint8>(i-1,j) + binImg.at<uint8>(i-1,j+1)
                + binImg.at<uint8>(i,j-1) + binImg.at<uint8>(i,j+1)
                + binImg.at<uint8>(i+1,j-1) + binImg.at<uint8>(i+1,j) + binImg.at<uint8>(i+1,j+1);
            if (num >= threshold_max && binImg.at<uint8>(i,j) == 0) {
                binImg.at<uint8>(i,j) = BIN_WHITE;//白  可以搞成宏定义，方便更改
            }
            if (num <= threshold_min && binImg.at<uint8>(i,j) == BIN_WHITE) {
                binImg.at<uint8>(i,j) = BIN_BLACK;//黑
            }
        }
    }
}

/*在二值图像的四周画一个厚度为2的矩形边框*/
void binImgDrawRect(cv::Mat& binImg) {
    for (int i=0; i<RESIZED_HEIGHT; i++) {
        binImg.at<uint8>(i,0) = 0;
        binImg.at<uint8>(i,1) = 0;
        binImg.at<uint8>(i,RESIZED_WIDTH - 1) = 0;
        binImg.at<uint8>(i,RESIZED_WIDTH - 2) = 0;
    }
    for (int i=0; i<RESIZED_WIDTH; i++) {
        binImg.at<uint8>(0,i) = 0;
        binImg.at<uint8>(1,i) = 0;
    }
}

/**
 * @brief 在二值图像上的左右边界绘制黑色边框
 * @param binImg 输入的二值图像
 */
void drawLeftRightBlackBorder(cv::Mat& binImg) {
    if (binImg.type() != CV_8UC1) { // 检查图像是否为二值图像
        std::cerr << "Image is not a binary image!" << std::endl;
        return;
    }
    int rows = binImg.rows;
    int cols = binImg.cols;
    for (int r=0; r<rows; r++) {
        binImg.at<uint8>(r,0) = BIN_BLACK; // 左边界
        binImg.at<uint8>(r,cols-1) = BIN_BLACK; // 右边界
    }
}

/**
 * @brief 在二值图像顶部和底部绘制黑色边框
 * @param binImg 二值图像
 */
void drawTopBottomBlackBorder(cv::Mat& binImg) {
    if (binImg.type() != CV_8UC1) { // 检查图像是否为二值图像
        std::cerr << "Image is not a binary image!" << std::endl;
        return;
    }
    int rows = binImg.rows;
    int cols = binImg.cols;
    for (int c=0; c<cols; c++) {
        binImg.at<uint8>(0,c) = BIN_BLACK; // 上边界
        binImg.at<uint8>(rows-1,c) = BIN_BLACK; // 下边界
    }
}

/**
 * @brief 对 cv::Point 容器中的元素的x坐标进行滤波
 * @note 滤波算法：在窗口内，对x坐标求平均
 * @param points 输入的点容器
 * @param filteredPoints 输出的滤波后的点容器
 * @param windowSize 滤波窗口大小，最好是奇数
 */
void filterXCoord(std::vector<cv::Point>& points, std::vector<cv::Point>& filteredPoints, int windowSize) {
    if (points.empty()) return; // 如果点容器为空，直接返回
    filteredPoints.clear(); // 清空输出的点容器
    int halfWindow = windowSize / 2; // 窗口的一半大小
    size_t pointsSize = points.size(); // 点容器的大小
    for (size_t i=0; i<pointsSize; i++) {
        int start = std::max(0, (int)i-halfWindow); // 窗口起始位置
        int end = std::min((int)pointsSize-1, (int)i+halfWindow); // 窗口结束位置
        int xFiltered = std::accumulate(
            points.begin() + start,
            points.begin() + end,
            0, [](int sum, const cv::Point& pt) { return (sum + pt.x); }
        ) / (end-start); // 计算平均值
        filteredPoints.push_back(cv::Point(xFiltered, points[i].y)); // 将平均值作为新的点加入输出容器
    }
}