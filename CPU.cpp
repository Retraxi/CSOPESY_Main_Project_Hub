#include "Core.h"
#include <thread>
#include <memory>
#include <mutex>
#include "Process.h"

int Core::nextID = 0;
int Core::msDelay = 50;

Core::Core() {
    this->_id = CPU::nextID;
    Core::nextID++;
    std::thread tickThread(&CPU::run, this);
    tickThread.detach();
}

void Core::setProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(this->mtx);
    if (this->_process != nullptr) {
        this->_process->setCPUCoreID(-1);
    }
    this->_process = process;
    this->_ready = process == nullptr;
}

void CPU::run() {
    this->_stopFlag = false;
    while (!this->_stopFlag) {
        this->execute();
        std::this_thread::sleep_for(std::chrono::milliseconds(Core::msDelay));
    }
    this->_ready = true;
}



void Core::execute() {
    std::lock_guard<std::mutex> lock(this->mtx);
    this->_totalTicks++;
    if (this->_process != nullptr && !this->_process->hasFinished()) {
        this->_process->setCPUCoreID(this->_id);
        this->_process->execute();
        if (this->_process->hasFinished()) {
            this->_ready = true;
        }
    }
    else {
        this->_inactiveTicks++;
    }
}
