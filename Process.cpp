
#include "Process.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include <random>
#include "PrintCommand.h"

typedef std::string String;

int Process::requiredPages = -1;
int Process::sameMemory = -1;

Process::Process(String name, std::uniform_int_distribution<int> commandDistr,
    std::uniform_int_distribution<int> memoryDistr,
    std::uniform_int_distribution<int> pageDistr) : _name(std::move(name)) {

    std::lock_guard<std::mutex> lock(mtx);
    this->_pid = Process::nextID++;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    int numCommands = commandDistr(generator);

    // Generate a list of commands
    for (int i = 0; i < numCommands; ++i) {
        this->_commandList.push_back(
            std::make_shared<PrintCommand>(
                "Executing from " + this->_name + "!", this->_pid
            )
        );
    }

    // Set required memory or use static allocation
    if (Process::sameMemory == -1) {
        this->_requiredMemory = calculatePowerOfTwo(memoryDistr(generator));
    }
    else {
        this->_requiredMemory = Process::sameMemory;
    }
}

void Process::execute() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!this->hasFinished()) {
        this->_commandList.at(_commandCounter)->execute(
            this->_cpuCoreID, ".\output\" + this->_name + ".txt"
        );
        ++this->_commandCounter;
    }
}

bool Process::hasFinished() const {
    return this->_commandCounter >= this->_commandList.size();
}

int Process::setRequiredPages(int min, int max) {
    if (Process::requiredPages == -1) {
        std::random_device rand_dev;
        std::mt19937 generator(rand_dev());
        std::uniform_int_distribution<int> pageDistr(min, max);
        Process::requiredPages = calculatePowerOfTwo(pageDistr(generator));
    }
    return Process::requiredPages;
}

int Process::setRequiredMemory(int min, int max) {
    if (Process::sameMemory == -1) {
        std::random_device rand_dev;
        std::mt19937 generator(rand_dev());
        std::uniform_int_distribution<int> memDistr(min, max);
        Process::sameMemory = calculatePowerOfTwo(memDistr(generator));
    }
    return Process::sameMemory;
}

void Process::setCPUCoreID(int cpuCoreID) {
    std::lock_guard<std::mutex> lock(mtx);
    this->_cpuCoreID = cpuCoreID;
}

int Process::calculatePowerOfTwo(int value) {
    int power = 1;
    while (power < value) {
        power *= 2;
    }
    return power;
}

int Process::nextID = 0;
