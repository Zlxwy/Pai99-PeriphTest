#ifndef __PIDOBJECT_HPP
#define __PIDOBJECT_HPP

// 如何使用？
// PidObject xlPID;
// xlPID.isPolOfMeaValCsstWithOutVal = true;
// xlPID.kP = 27.0;
// xlPID.kI = 0.0;
// xlPID.kD = 56.0;
// xlPID.isFirstOrderFilterEnabled = false;
// xlPID.filterParam = 0.0;
// xlPID.targetVal = 0.0;
// xlPID.measuredVal = 0.0;
// xlPID.isErrorLimitEnabled = true;
// xlPID.errorLimit[0] = xlPID.targetVal - 0.7;
// xlPID.errorLimit[1] = xlPID.targetVal + 0.7;
// xlPID.isIntegLimitEnabled = true;
// xlPID.integLimit[0] = -70.0;
// xlPID.integLimit[1] = +70.0;
// xlPID.isOutputEnabled = true;
// xlPID.measuredVal = xl; // 将xl的值赋给xlPID的测量值
// 最后取xlPID.pidCalculate()的返回值，或者直接访问成员变量 outputVal

/**
 * @brief 需要进行PID运算的对象
 */
class PidObject {
public:
    bool isPolOfMeaValCsstWithOutVal = false; // Is the polarity of measured value consistent with output value? 测量值和输出值的极性是否一致
    // 比如测量值相对于目标值为负方向时，需要一个负数的输出值才能纠正，此时 isPolOfMeaValCsstWithOutVal 为 true
    double kP = 0; // Proportional Gain 比例增益
    double kI = 0; // Proportional Gain 比例增益
    double kD = 0; // Proportional Gain 比例增益


    double targetVal = 0; // Target Value 目标值
    double measuredVal = 0; // Measured Value 测量值
    bool isErrorLimitEnabled = false; // Is the error limit enabled? 是否启用误差限幅
    // 如果启用，则 currError = valLimit(currError, errorLimit[0], errorLimit[1])
    double errorLimit[2] = {0, 0}; // Error Limit 误差限幅，[0]是最小值，[1]是最大值
    bool isFirstOrderFilterEnabled = false; // Is the first order filter enabled? 一阶滤波器是否启用
    // 如果启用，则 currError = currError * (1 - filterParam) + prevError * filterParam
    double filterParam = 0;  // 上次误差 prevError 的权重，用于一阶滤波
    double currError = 0; // Current Error 当前误差
    double prevError = 0; // Previous Error 上次误差
    
    bool isIntegLimitEnabled = false; // Is the integral limit enabled? 是否启用积分限幅
    // 如果启用，则 errorInteg = valLimit(errorInteg, integLimit[0], integLimit[1])
    double integLimit[2] = {0, 0}; // Integral Limit 积分限幅，[0]是最小值，[1]是最大值
    double errorInteg = 0; // Error Integral 误差积分
    double errorDeriv = 0; // Error Derivative 误差微分

    bool isOutputEnabled = false; // Is the output enabled? 是否启用输出
    // 如果启用，则 outputVal = pidCalculate()，否则 outputVal = 0
    double outputVal = 0; // Output Value 输出值，调用 pidCalculate 方法后，此数值将会更新
    double pidCalculate(void); // 根据给定的PID对象，计算PID控制器的输出值。
    double valLimit(double val, double min, double max); // 限制val在[min,max]之间
};

#endif // #ifndef __PIDOBJECT_HPP