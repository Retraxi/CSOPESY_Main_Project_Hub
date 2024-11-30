
#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <random>
#include "ICommand.h"

class Process {
public:
    Process(std::string name,
        std::uniform_int_distribution<int> commandDistr,
        std::uniform_int_distribution<int> memoryDistr,
        std::uniform_int_distribution<int> pageDistr
    );
    ~Process() = default;

    void execute();
    bool hasFinished() const;

    int getID() const { return _pid; };
    std::string getName() const { std::lock_guard<std::mutex> lock(mtx); return _name; };
    int getCommandCounter() const { std::lock_guard<std::mutex> lock(mtx); return _commandCounter; };
    int getCommandListSize() const { std::lock_guard<std::mutex> lock(mtx); return _commandList.size(); };
    int getBurst() const { return getCommandListSize() - getCommandCounter(); };
    time_t getArrivalTime() const { return _arrivalTime; };
    time_t getFinishTime() const { return _finishTime; };
    int getRequiredMemory() const { std::lock_guard<std::mutex> lock(mtx); return _requiredMemory; };
    static int setRequiredPages(int min, int max);
    static int setRequiredMemory(int min, int max);
    static int getRequiredPages() { return Process::requiredPages; };
    int getCPUCoreID() const { std::lock_guard<std::mutex> lock(mtx); return _cpuCoreID; };

    void setCPUCoreID(int cpuCoreID);
    void setFinishTime() { _finishTime = time(nullptr); };

    bool operator<(const std::shared_ptr<Process>& other) const {
        return getBurst() > other->getBurst();
    };

    static int nextID;

private:
    mutable std::mutex mtx;

    int _pid;
    std::string _name;
    std::vector<std::shared_ptr<ICommand>> _commandList;
    int _commandCounter = 0;
    int _cpuCoreID = -1;
    time_t _arrivalTime = time(nullptr);
    time_t _finishTime = time(nullptr);

    int _requiredMemory;
    static int requiredPages;
    static int sameMemory;

    static int calculatePowerOfTwo(int value); // Utility function to round up to the nearest power of two
};

#endif // !PROCESS_H
