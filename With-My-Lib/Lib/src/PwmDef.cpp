#include "PwmDef.hpp"

PwmDef::PwmDef(const string &mold, const string &channel, int period, int duty_cycle, const string &polarity, bool is_enable) {
    this->_fd = 0;
    this->_Mold = mold;
    this->_Channel = channel; // 暂时不知道这个"0"指定的pwmchip0/pwm0是什么
    this->_Period = period;
    this->_Duty_Cycle = duty_cycle;
    this->_Polarity = polarity;
    this->_is_enable = is_enable;
    if(this->pwmExport() != 0) {
        cerr << "第一次错误：因先前配置未清除，属正常" << endl;
        this->pwmUnexport();
        if(this->pwmExport() != 0)
            cerr << "第二次错误：异常！！！" << endl;
        cout << "未产生第二次错误，PWM引脚已配置成功！" << endl;
    } else {
        cout << this->_Mold << "/pwm" << this->_Channel << "引脚已正常配置！" << endl;
    }
    this->setPeriod(this->_Period);
    this->setDuty(this->_Duty_Cycle);
    this->setPolarity(this->_Polarity);
    this->setCmd(this->_is_enable);
}

int PwmDef::pwmExport(void) {
    /*
        echo 0 > /sys/class/pwm/pwmchip0/export
    */
    string path = "/sys/class/pwm/" + this->_Mold + "/export";
    this->_fd = this->openFile(path, O_WRONLY);
    if(this->_fd < 0) {
        cerr << "无法打开export文件" << endl;
        return -1;
    }
    // 若文件打开成功，来到这里
    if(this->writeFile(this->_fd, this->_Channel) < 0) {
        cerr << "无法向export文件写入" << endl;
        return -1;
    }
    return 0;
}

int PwmDef::pwmUnexport(void) {
    /*
        echo 0 > /sys/class/pwm/pwmchip0/unexport
    */
   string path = "/sys/class/pwm/" + this->_Mold + "/unexport";
   this->_fd = this->openFile(path, O_WRONLY);
   if(this->_fd < 0) {
       cerr << "无法打开unexport文件" << endl;
       return -1;
   }
   if(this->writeFile(this->_fd, this->_Channel) < 0) {
       cerr << "无法向unexport文件写入" << endl;
       return -1;
   }
   return 0;
}

int PwmDef::setPeriod(int period) {
    /*
        echo 100000 > /sys/class/pwm/pwmchip0/pwm0/period
    */
    string path = "/sys/class/pwm/" + this->_Mold + "/pwm" + this->_Channel + "/period";
    this->_fd = this->openFile(path, O_RDWR);
    if(this->_fd < 0) {
        cerr << "无法打开period文件" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, to_string(period)) < 0) {
        cerr << "无法向period写入周期值" << endl;
        return -1;
    }
    this->_Period = period;
    return 0;
}

int PwmDef::setDuty(int duty_cycle) {
    /*
        echo 1000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
    */
    string path = "/sys/class/pwm/" + this->_Mold + "/pwm" + this->_Channel + "/duty_cycle";
    this->_fd = this->openFile(path, O_RDWR);
    if(this->_fd < 0) {
        cerr << "无法打开duty_cycle文件" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, to_string(duty_cycle)) < 0) {
        cerr << "无法向duty_cycle文件写入占空值" << endl;
        return -1;
    }
    this->_Duty_Cycle = duty_cycle;
    return 0;
}

int PwmDef::setPolarity(const string &polarity) {
    /*
        echo "inversed" > /sys/class/pwm/pwmchip0/pwm0/polarity
        echo "normal" > /sys/class/pwm/pwmchip0/pwm0/polarity
    */
    string path = "/sys/class/pwm/" + this->_Mold + "/pwm" + this->_Channel + "/polarity";
    this->_fd = this->openFile(path, O_RDWR);
    if(this->_fd < 0) {
        cerr << "无法打开polarity文件" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, polarity) < 0) {
        cerr << "无法向polarity文件写入数据" << endl;
        return -1;
    }
    this->_Polarity = polarity;
    return 0;
}

int PwmDef::setCmd(bool is_enable) {
    /*
        echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
        echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable
    */
    string path = "/sys/class/pwm/" + this->_Mold + "/pwm" + this->_Channel + "/enable";
    this->_fd = this->openFile(path, O_RDWR);
    if(this->_fd < 0) {
        cerr << "无法打开enable文件" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, to_string((int)is_enable)) < 0) {
        cerr << "无法向enable文件写入数据" << endl;
        return -1;
    }
    this->_is_enable = is_enable;
    return 0;
}

PwmDef::~PwmDef() {
    this->setCmd(false);
    this->pwmUnexport();
}