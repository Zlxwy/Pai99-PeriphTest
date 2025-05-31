#include "Timer.hpp"

/**
 * @brief Timer 类的构造函数
 * @param interval_ms 定时任务的间隔时间（毫秒）
 * @param task 定时任务要执行的函数，该函数无参数且无返回值
 * @param isOnImdtly 是否立即启动任务，如果为 true 则立即启动任务，否则不启动
 */
Timer::Timer(int interval_ms, std::function<void(void)> task, bool isOnImdtly) { // is on immediately? 是否立即打开线程定时器
    this->interval = interval_ms;
    this->task = task;
    this->exitInfo = "有个线程退出了。";

    if (isOnImdtly) this->start();
    else this->isRunning.store(false);
}

Timer::~Timer() {
    this->stop(); // 确保定时器在结束后能被销毁，线程都被释放
}

void Timer::start(void) {
    this->isRunning.store(true); // 将线程运行标志位置true
    this->worker = std::thread(&Timer::run, this); // 创建一个线程用于运行run方法
}

void Timer::stop(void) {
    this->isRunning.store(false); // 将线程运行标志位置false
    if ( this->worker.joinable() ) { this->worker.join(); } // 阻塞等待线程结束
}

void Timer::setExitInfo(const std::string &info) {
    this->exitInfo.assign(info);
}

void Timer::run(void) {
    while ( this->isRunning.load() ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(this->interval));
        if ( this->isRunning.load() ) {
            // std::thread(this->task).detach(); // 使用这条语句执行task函数，如果在task函数未运行完又来一个新的线程，会有危险
            this->task(); // 使用这条语句执行task函数，会在task函数运行完后才会进行线程延时
        }
    }
    std::cout << this->exitInfo << std::endl; // 打印线程退出信息
}
