#include <iostream>
#include "PIDObject.hpp"

/**
 * @brief 根据给定的PID对象，计算PID控制器的输出值。
 */
double PidObject::pidCalculate(void) {
    this->currError = (this->isPolOfMeaValCsstWithOutVal == true) ?
                      (this->measuredVal - this->targetVal) :
                      (this->targetVal - this->measuredVal) ;
    /**
        在PID控制器中，误差的处理方式（先限幅后一阶滤波，还是先一阶滤波后限幅）取决于具体的应用场景和控制目标。以下是两种方法的优缺点分析：
        1. 先限幅后一阶滤波
        - 优点：
            - 防止滤波器饱和： 如果误差信号非常大，直接进入滤波器可能会导致滤波器饱和，从而影响其性能。限幅可以防止这种情况发生。
            - 保护系统： 在某些情况下，过大的误差信号可能会对系统造成损害，限幅可以起到保护作用。
        缺点：
            - 信息丢失： 限幅会丢失部分误差信息，可能导致控制精度下降。
            - 滤波效果受限： 由于限幅后的信号已经丢失了一部分信息，滤波器的效果可能会受到影响。
        2. 先一阶滤波后限幅
        - 优点：
            - 平滑误差信号： 滤波器可以平滑误差信号，减少噪声和波动，提高控制精度。
            - 保留更多信息： 滤波后再限幅可以保留更多的误差信息，有助于提高控制效果。
        - 缺点：
            - 滤波器饱和风险： 如果误差信号过大，滤波器可能会饱和，导致其性能下降。
            - 系统保护不足： 在某些情况下，过大的误差信号可能会对系统造成损害，滤波后再限幅可能无法及时保护系统。
        
        选择建议
        - 系统保护和安全优先： 如果系统的安全性和保护是首要考虑因素，建议先限幅后滤波。这样可以确保误差信号在进入滤波器之前不会过大，从而保护系统和滤波器。
        - 控制精度和性能优先： 如果控制精度和性能是首要考虑因素，建议先滤波后限幅。这样可以最大限度地保留误差信息，提高控制效果。
        （来自智譜清言AI大模型ChatGLM）
    */
    this->currError = (this->isErrorLimitEnabled == true) ? // 是否需要误差限幅
                      (this->valLimit(this->currError, this->errorLimit[0], this->errorLimit[1])) :
                      (this->currError) ;
    this->currError = (this->isFirstOrderFilterEnabled == true) ? // 是否需要一阶滤波
                      (this->currError * (1 - this->filterParam) + this->prevError * this->filterParam) :
                      (this->currError) ;
    this->errorInteg += this->currError;
    this->errorInteg = (this->isIntegLimitEnabled == true) ? // 是否需要积分限幅
                      (this->valLimit(this->errorInteg, this->integLimit[0], this->integLimit[1])) :
                      (this->errorInteg) ;
    this->errorDeriv = this->currError - this->prevError;
    this->prevError = this->currError;
    this->outputVal = (this->isOutputEnabled == true) ? // 是否使能输出
                      (this->kP * this->currError + this->kI * this->errorInteg + this->kD * this->errorDeriv) :
                      (0) ;
    return this->outputVal;
}

/**
 * @brief 限制val在[min,max]之间
 */
double PidObject::valLimit(double val, double min, double max) {
    return (val < min) ? min : ( (val > max) ? max : val );
}