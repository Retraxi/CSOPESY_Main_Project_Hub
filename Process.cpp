#define _CRT_SECURE_NO_WARNINGS
#include "Process.h"
#include <iostream>
#include <iomanip>
#include <cmath> // For std::ceil
#include <utility> // For std::move

// Constructor for processes without memory size
Process::Process(int pid, std::string name)
    : pid(pid), processName(std::move(name)), memorySize(0), coreID(-1), currentInstructionLine(0) {
    std::time_t now = std::time(nullptr);
    createdAt = std::localtime(&now);
    finishedAt = nullptr;
}

// Constructor for processes with memory size
Process::Process(int pid, std::string name, size_t memorySize)
    : pid(pid), processName(std::move(name)), memorySize(memorySize), coreID(-1), currentInstructionLine(0) {
    std::time_t now = std::time(nullptr);
    createdAt = std::localtime(&now);
    finishedAt = nullptr;
}

// Getters
std::string Process::getName() const {
    return processName;
}

int Process::getCoreID() const {
    return coreID;
}

int Process::getProcessID() const {
    return pid;
}

int Process::getTotalInstructionLines() const {
    return instructionList.size();
}

int Process::getCurrentInstructionLines() const {
    return currentInstructionLine;
}

int Process::getCommandCounter() const {
    return commandCounter;
}

void Process::setMemorySize(size_t memorySize) {
    if (memorySize <= 0) {
        this->memorySize = 0;
    }
    else {
        this->memorySize = memorySize;
    }
}



size_t Process::getMemorySize() const {
    return memorySize;
}
// Time-related methods
tm* Process::getCreatedAt() {
    return createdAt;
}

tm* Process::getFinishedAt() {
    return finishedAt;
}

void Process::setFinishedAt(tm* finishedAt) {
    std::lock_guard<std::mutex> lock(mtx);
    this->finishedAt = finishedAt;
}

// Methods for instructions
void Process::testInitFCFS() {
    std::string instruction = "Test instruction from " + processName;
    for (size_t i = 0; i < 100; i++) {
        instructionList.push_back(instruction);
    }
}

void Process::setInstruction(int totalCount) {
    std::string instruction = "Instruction from " + processName;
    for (int i = 0; i < totalCount; i++) {
        instructionList.push_back(instruction);
    }
}

// Check if the process is done executing
bool Process::isDone() {
    if (currentInstructionLine >= instructionList.size()) {
        std::time_t now = std::time(nullptr);
        setFinishedAt(std::localtime(&now));
        return true;
    }
    return false;
}

// Set the core ID for the process
void Process::setCoreID(int coreID) {
    std::lock_guard<std::mutex> lock(mtx);
    this->coreID = coreID;
}

// Simulate process execution
void Process::execute() {
    std::time_t now = std::time(nullptr);
    int dummy = 0; // Simulated workload

    if (!isDone()) {

        // Simulate processing delay
        for (size_t i = 0; i < 10000; i++) {
            dummy += 1;
        }

        currentInstructionLine++;

        if (isDone()) {
        }
    }
    else {
    }
}

void Process::setFrameIndices(std::vector<size_t> frameIndices) {
    this->frameIndices = frameIndices;
}

std::vector<size_t> Process::getFrameIndices() {
    return frameIndices;
}

std::vector<char> memory;
