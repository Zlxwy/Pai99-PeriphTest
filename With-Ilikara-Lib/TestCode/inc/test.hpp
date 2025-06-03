#ifndef __TESTCODE_HPP
#define __TESTCODE_HPP
#include <opencv2/opencv.hpp> // 包含OpenCV库的头文件，用于图像处理。
#include <string>

// 类型
typedef uint8_t     uint8;  // 8位无符号整数
typedef uint16_t    uint16; // 16位无符号整数
typedef uint32_t    uint32; // 32位无符号整数
typedef uint64_t    uint64; // 64位无符号整数
typedef int8_t      int8;   // 8位有符号整数
typedef int16_t     int16;  // 16位有符号整数
typedef int32_t     int32;  // 32位有符号整数
typedef int64_t     int64;  // 64位有符号整数

// 相机索引，一般为0，有时候为1，如果有时候相机打不开，在这里更改一下
#define CAM_INDEX           0
#define CAM_WIDTH           640 // 实际测试的时候，发现这宽度和高度只能设置为(640*480)和(1920*1080)
#define CAM_HEIGHT          480
#define RESIZED_WIDTH       128 // 缩放后的宽度
#define RESIZED_HEIGHT      96 // 缩放后的高度
#define ROW_UP              50 // 计算路径时，从第几行开始
#define ROW_DOWN            70 // 计算路径时，到第几行结束
#define GRAY2BIN_THRESH     119 // 灰度图像转黑白图像的灰度阈值
#define MIN_AREA_THRESH     90 // 黑白图像移除面积较小的孤岛色块的面积阈值
#define BIN_WHITE           255 // 二值图像的白色
#define BIN_BLACK           0   // 二值图像的黑色
#define BGR_WHITE           255,255,255
#define BGR_BLACK           0,0,0
#define BGR_BLUE            255,0,0
#define BGR_GREEN           0,255,0
#define BGR_RED             0,0,255
#define BGR_PURPLE          128,0,128
#define BGR_ICEBLUE         255,255,0



/*运动器件的一些定义*/
#define PWM_CLK_FREQ    ((int)1000000000)
#define MOTOR_PWM_FREQ  ((int)20000)
#define MOTOR_CNT_MAX   ((int)(PWM_CLK_FREQ/MOTOR_PWM_FREQ))
#define SERVO_PWM_FREQ  ((int)50)
#define SERVO_CNT_MAX   ((int)(PWM_CLK_FREQ/SERVO_PWM_FREQ))

#define SERVO_ANGLE_MID  ((double)83.0)        // 舵机的中值角度
#define SERVO_OFFSET_ANGLE_MAX ((double)22.0)  // 舵机可偏移的最大角度
#define SERVO_ANGLE_MAX  (SERVO_ANGLE_MID+SERVO_OFFSET_ANGLE_MAX) // 舵机的最大角度，83+22=105
#define SERVO_ANGLE_MIN  (SERVO_ANGLE_MID-SERVO_OFFSET_ANGLE_MAX) // 舵机的最小角度，83-22=61

#define ms2us(ms)       (long)(ms*1000) // 毫秒(millisecond)转微秒(microsecond)
#define sec2us(sec)     (long)(sec*1000000) // 秒(second)转微秒(microsecond)



/*在测试的时候发现，第一种清空终端方式可以上到20帧，第二种在16 17帧左右。*/
#define clearScreen_ESCAPE     printf("\x1B[2J\x1B[H") // 使用ANSI escape code清空终端
#define clearScreen_COMMAND    system("clear") // 使用系统命令清空终端



/*一些关于硬件驱动的辅助函数==============================================================================*/
int angle2cnt(double angle, int cntMAX); // 角度值转占空比计数值
int speedPercent2cnt(double speedPercent, int cntMAX); // 速度百分比转占空比计数值
double mapArea(double origValMin, double origValMax, double newValMin, double newValMax, double origVal);
/*一些关于linux系统下的辅助函数*/
template <typename T> T readNumFromFile(const std::string& filename); // 从文本文件中读取出一个数值
double myClamp(const double val, const double low, const double high); // 自己写的一个范围限制函数
int myClamp(const int val, const int low, const int high); // 自己写的一个范围限制函数
/*一些关于图像处理的辅助函数*/
double bgr2bin(const cv::Mat& bgrImg, cv::Mat& resizedImg, cv::Mat& grayImg, cv::Mat& binImg,
               const cv::Size& imgSize, double thresh, double maxval);// 指定阈值，将彩色图像转为指定像素的二值图像
double bgr2bin(const cv::Mat& bgrImg, cv::Mat& resizedImg, cv::Mat& grayImg, cv::Mat& binImg,
               const cv::Size& imgSize);// 使用自动阈值，将彩色图像转为指定像素的二值图像，返回值为转换为二值化图像时的阈值
bool isBinImgRectRoiAllColor(const cv::Mat& binImg, const cv::Rect& rect, uint8 color);
/*对于以下两个寻找小块函数，需要说明的是，在嵌套内的小块的坐标并不会被加入容器*/
void getWhiteCentersOfSmallRegions(const cv::Mat& binImg, double minArea, std::vector<cv::Point>& centers); // 获取二值图像中面积小于指定值的独立白色区域
void getBlackCentersOfSmallRegions(const cv::Mat& binImg, double minArea, std::vector<cv::Point>& centers); // 获取二值图像中面积小于指定值的独立黑色区域
void fillBlobRecursive(cv::Mat& binImg, cv::Point pt, uint8 newColor); // 洪水填充二值图像某一区域的颜色，就像Windows画图软件的倒墨水
void fillBlobRecursive(cv::Mat& bgrImg, cv::Point pt, cv::Vec3b newColor, cv::Vec3b oldColor); // 洪水填充彩色图某一区域的颜色，就像Windows画图软件的倒墨水
void findEdgePix(const cv::Mat binImg, std::vector<cv::Point>& leftEdgePoints, std::vector<cv::Point>& rightEdgePoints); // 传入一张二值化图像，并将左右的边界点压入到两个容器中
cv::Point calMidPoint(const cv::Point pt1, const cv::Point& pt2); // 计算两个坐标之间的中点坐标
void drawLineOnRow(cv::Mat& img, int rowIndex, cv::Scalar color, int thickness); // 传入一张图像，在指定行上绘制指定颜色厚度的直线
double myCalAverage(const std::vector<double> vdata); // 自己写的一个计算容器中所有数据的平均值
template <typename T> T calAverage(const std::vector<T> vdata); // 计算一个容器中所有数据的平均值（来自智譜清言AI）
double calAverSlopeFromRowToRow(cv::Mat& img, std::vector<cv::Point> midPoint, int row_up, int row_down); // 计算行到行之间点的斜率
double calAverXCoordFromRowToRow(cv::Mat& img, std::vector<cv::Point> midPoint, int row_up, int row_down); // 计算行到行之间x坐标平均值
void drawBlackRectBorder(cv::Mat& binImg); // 在二值图像四周绘制一个厚度为1的黑色矩形边框
void invertSmallRegion_1(cv::Mat& binImg, double minArea); // 反转二值图像中面积小于指定值的独立区域，方法1
void invertSmallRegion_2(cv::Mat& binImg, double minArea); // 反转二值图像中面积小于指定值的独立区域，方法2
void binImgFilter(cv::Mat& binImg); // 二值图像滤波
void binImgDrawRect(cv::Mat& binImg); // 在二值图像的四周画一个厚度为2的矩形边框
void drawLeftRightBlackBorder(cv::Mat& binImg); // 在二值图像的左右画黑线
void drawTopBottomBlackBorder(cv::Mat& binImg); // 在二值图像的上下画黑线
void filterXCoord(std::vector<cv::Point>& points, std::vector<cv::Point>& filteredPoints, int windowSize); // 滤波器，平滑x坐标值

/*放在主函数中的测试函数==============================================================================*/
int test_motor_drive(void); // 电机整体功能初始化代码
int test_pwm_output(void); // 测试PWM输出
int test_gpio_output(void); // 测试GPIO输出
int test_read_num_from_file(void); // 测试从文件中读取数值
int test_read_distance(void); // 测试读取vl53l0x测量的距离数值
int test_wonderEcho_sound(void); // 测试wonderEcho声音模块

int test_keyboard_ctrl_servo(void); // 测试键盘控制舵机
int test_keyboard_ctrl_car(void); // 测试键盘控制小车
int test_ocv_ctrl_car_with_pid(void); // 测试OpenCV控制小车，使用PID控制舵机转向
int test_vl53l0x_obtc_avds(void); // 测试vl53l0x测距模块

#endif // #ifndef __TESTCODE_HPP