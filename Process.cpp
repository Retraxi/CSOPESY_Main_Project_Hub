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

    std::cout << "Process Created: ID=" << pid << ", Name=" << processName
        << ", No Memory Size Allocated.\n";
}

// Constructor for processes with memory size
Process::Process(int pid, std::string name, size_t memorySize)
    : pid(pid), processName(std::move(name)), memorySize(memorySize), coreID(-1), currentInstructionLine(0) {
    std::time_t now = std::time(nullptr);
    createdAt = std::localtime(&now);
    finishedAt = nullptr;

    std::cout << "Process Created: ID=" << pid << ", Name=" << processName
        << ", Memory Size=" << memorySize << " bytes (" << getNumPages() << " pages).\n";
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

size_t Process::getMemorySize() const {
    return memorySize;
}

size_t Process::getNumPages() const {
    return (memorySize + PAGE_SIZE - 1) / PAGE_SIZE; // Round up to calculate pages
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
    std::cout << "Process ID=" << pid << " marked as finished at "
        << std::put_time(finishedAt, "%c") << ".\n";
}

// Methods for instructions
void Process::testInitFCFS() {
    std::string instruction = "Test instruction from " + processName;
    for (size_t i = 0; i < 100; i++) {
        instructionList.push_back(instruction);
    }
    std::cout << "Process ID=" << pid << ": Initialized with 100 test instructions.\n";
}

void Process::setInstruction(int totalCount) {
    std::string instruction = "Instruction from " + processName;
    for (int i = 0; i < totalCount; i++) {
        instructionList.push_back(instruction);
    }
    std::cout << "Process ID=" << pid << ": Initialized with " << totalCount << " instructions.\n";
}

// Check if the process is done executing
bool Process::isDone() {
    if (currentInstructionLine >= instructionList.size()) {
        std::time_t now = std::time(nullptr);
        setFinishedAt(std::localtime(&now));
        std::cout << "Process ID=" << pid << " has completed execution.\n";
        return true;
    }
    return false;
}

// Set the core ID for the process
void Process::setCoreID(int coreID) {
    std::lock_guard<std::mutex> lock(mtx);
    this->coreID = coreID;
    std::cout << "Process ID=" << pid << " assigned to Core ID=" << coreID << ".\n";
}

// Simulate process execution
void Process::execute() {
    std::time_t now = std::time(nullptr);
    int dummy = 0; // Simulated workload

    if (!isDone()) {
        std::cout << "Process ID=" << pid << " executing instruction " << currentInstructionLine + 1
            << "/" << instructionList.size() << ".\n";

        // Simulate processing delay
        for (size_t i = 0; i < 10000; i++) {
            dummy += 1;
        }

        currentInstructionLine++;

        if (isDone()) {
            std::cout << "Process ID=" << pid << " has finished executing all instructions.\n";
        }
    }
    else {
        std::cout << "Process ID=" << pid << " is already completed.\n";
    }
}
