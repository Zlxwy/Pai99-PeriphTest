#include "GpioDef.hpp"

GpioDef::GpioDef() {
    this->_fd = 0;
    this->_Gpio = -1;
    this->_Dir = MODE_OUT;
}

GpioDef::GpioDef(int gpio, const string &dir) {
    this->_fd = 0;
    this->_Gpio = gpio;
    this->_Val = 1;
    this->_Dir = dir;
    if(this->GpioExport() != 0) { // 如果创建失败
        cerr << "第一次错误：因先前配置未清除，属正常" << endl;
        this->GpioUnexport(); // 有可能之前创建过了，释放一下配置
        if(this->GpioExport() != 0) // 重新创建
            cerr << "第二次错误：异常！！！" << endl;
        cout << "未产生第二次错误，GPIO引脚已配置成功！" << endl;
    }
    else {
        cout << "GPIO" << this->_Gpio << "已正常配置！" << endl;
    }
    this->setDir(this->_Dir);
}

int GpioDef::GpioExport() { // 将指定的 GPIO（通用输入输出）引脚导出
    /*
        echo xx > /sys/class/gpio/export
    */
    string path = "/sys/class/gpio/export";
    this->_fd = this->openFile(path, O_WRONLY);
    if(this->_fd < 0) {
        cerr << "打开export文件失败" << endl; // 输出错误信息
        return -1; // 返回-1
    }
    if(this->writeFile(this->_fd, to_string(this->_Gpio)) < 0) {
        cerr << " 导出gpio" << this->_Gpio << "失败" << endl; // 输出错误信息
        return -1; // 返回-1
    }
    return 0;

    // this->Gpio = gpio; // 将该编号保存到成员变量this->Gpio中
    // this->fd = open("/sys/class/gpio/export", O_WRONLY); // 以只写模式打开/sys/class/gpio/export文件
    // if(this->fd < 0) { // 如果打开失败
    //     cerr << "打开export文件失败" << endl; // 输出错误信息
    //     return -1; // 返回-1
    // }
    // string str = to_string(gpio);  // 将GPIO编号转换为字符串
    // int ret = write(this->fd, str.c_str(), str.size()); // 写入到已打开的export文件中
    // if(ret != str.size()) { // 若写入的字节数与字符串长度不符
    //     cerr << " 导出gpio" << gpio << "失败" << endl; // 输出错误信息
    //     close(this->fd); // 关闭文件
    //     return -1; // 返回-1
    // }
    // close(this->fd); // 如果一切顺利，关闭文件
    // return 0; // 返回0
}

int GpioDef::GpioUnexport() {
    /*
        echo xx > /sys/class/gpio/unexport
    */
    string path = "/sys/class/gpio/unexport";
    this->_fd = this->openFile(path, O_WRONLY);
    if(this->_fd < 0) {
        cerr << "打开unexport文件失败" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, to_string(this->_Gpio)) < 0) {
        cerr << "取消导出gpio" << this->_Gpio << "失败" << endl;
        return -1;
    }
    return 0;

    // this->fd = open("/sys/class/gpio/unexport", O_WRONLY);
    // if(this->fd < 0) {
    //     cerr << "打开unexport文件失败" << endl;
    //     return -1;
    // }
    // string str = to_string(this->Gpio);
    // int ret = write(this->fd, str.c_str(), str.size());
    // if(ret != str.size()) {
    //     cerr << "取消导出gpio" << this->Gpio << "失败" << endl;
    //     close(this->fd);
    //     return -1;
    // }
    // close(this->fd);
    // return 0;
}

int GpioDef::setDir(const string &dir) {
    /*
        echo "out" > /sys/class/gpio/gpioxx/direction
        echo "in" > /sys/class/gpio/gpioxx/direction
    */
    string path = "/sys/class/gpio/gpio" + to_string(this->_Gpio) + "/direction";
    this->_fd = this->openFile(path, O_WRONLY);
    if(this->_fd < 0) {
        cerr << "打开direction文件失败" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, dir) < 0) {
        cerr << "设置gpio为" << dir << "模式失败" << endl;
        return -1;
    }
    this->_Dir = dir;
    return 0;

    // string str = "/sys/class/gpio/gpio" + to_string(this->Gpio) + "/direction";
    // this->fd = open(str.c_str(), O_WRONLY);
    // if(this->fd < 0) {
    //     cerr << "打开direction文件失败" << endl;
    //     return -1;
    // }
    // int ret = write(this->fd, dir.c_str(), dir.size());
    // if(ret != dir.size()) {
    //     cerr << "设置gpio为" << dir << "模式失败" << endl;
    //     close(this->fd);
    //     return -1;
    // }
    // close(this->fd);
    // return 0;
}

int GpioDef::setVal(int val) {
    /*
        echo 1 > /sys/class/gpio/gpioxx/value
        echo 0 > /sys/class/gpio/gpioxx/value
    */
    string path = "/sys/class/gpio/gpio" + to_string(this->_Gpio) + "/value";
    this->_fd = this->openFile(path, O_WRONLY);
    if(this->_fd < 0) {
        cerr << "打开value文件失败" << endl;
        return -1;
    }
    if(this->writeFile(this->_fd, to_string(val)) < 0) {
        cerr << "设置gpio电平为" << val << "失败" << endl;
        return -1;
    }
    this->_Val = val;
    return 0;
    
    // string str = "/sys/class/gpio/gpio" + to_string(this->Gpio) + "/value";
    // this->fd = open(str.c_str(), O_WRONLY);
    // if(this->fd < 0) {
    //     cerr << "打开value文件失败" << endl;
    //     return -1;
    // }
    // int ret = write(this->fd, to_string(val).c_str(), to_string(val).size());
    // if(ret != to_string(val).size()) {
    //     cerr << "设置gpio电平为" << val << "失败" << endl;
    //     close(this->fd);
    //     return -1;
    // }
    // close(this->fd);
    // return 0;
}

int GpioDef::getVal() {
    /*
        cat /sys/class/gpio/gpioxx/value
    */
    string path = "/sys/class/gpio/gpio" + to_string(this->_Gpio) + "/value";
    this->_fd = this->openFile(path, O_RDONLY);
    if(this->_fd < 0) {
        cerr << "打开value文件失败" << endl;
        return -1;
    }
    char *read_data = new char[1];
    if(this->readFile(this->_fd, read_data, 1) != 1 ) {
        cerr << "读取gpio值失败" << endl;
        return -1;
    }
    char ret = read_data[0];
    delete[] read_data;
    return ret - '0'; // 将读取的字符转换为整数

    // string str = "/sys/class/gpio/gpio" + to_string(this->Gpio) + "/value";
    // this->fd = open(str.c_str(), O_RDONLY);
    // if(this->fd < 0) {
    //     cerr << "打开value文件失败" << endl;
    //     return -1;
    // }
    // char value;
    // int ret = read(this->fd, &value, 1);
    // if(ret != 1) {
    //     cerr << "读取gpio值失败" << endl;
    //     close(this->fd);
    //     return -1;
    // }
    // close(this->fd);
    // return value - '0'; // 将读取的字符转换为整数
}

GpioDef::~GpioDef() {
    if(this->_Gpio != -1) {
        GpioUnexport(); // 确保GPIO被取消导出
    }
}