#include "Process.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <utility>
#include <ctime>

// Helper function to calculate number of pages needed based on memory size and page size
constexpr size_t pageSize = 4096;  // Define the page size once, used throughout
size_t calculateNumPages(size_t memorySize) {
    return (memorySize + pageSize - 1) / pageSize;  // Using ceil logic without cmath
}

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
              << ", Memory Size=" << memorySize << " bytes (" << calculateNumPages(memorySize) << " pages).\n";
}

// Getters
std::string Process::getName() const {
    std::lock_guard<std::mutex> lock(mtx);
    return processName;
}

int Process::getCoreID() const {
    std::lock_guard<std::mutex> lock(mtx);
    return coreID;
}

int Process::getProcessID() const {
    return pid;
}

int Process::getTotalInstructionLines() const {
    std::lock_guard<std::mutex> lock(mtx);
    return instructionList.size();
}

int Process::getCurrentInstructionLines() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentInstructionLine;
}

int Process::getCommandCounter() const {
    std::lock_guard<std::mutex> lock(mtx);
    return commandCounter;
}

void Process::setMemorySize(size_t memorySize) {
    std::lock_guard<std::mutex> lock(mtx);
    this->memorySize = memorySize;
}

size_t Process::getMemorySize() const {
    return memorySize;
}

size_t Process::getNumPages() const {
    return calculateNumPages(memorySize);
}

// Time-related methods
std::tm* Process::getCreatedAt() const {
    return createdAt;
}

std::tm* Process::getFinishedAt() const {
    return finishedAt;
}

void Process::setFinishedAt(std::tm* finishedAt) {
    std::lock_guard<std::mutex> lock(mtx);
    this->finishedAt = finishedAt;
    std::cout << "Process ID=" << pid << " marked as finished at "
              << std::put_time(finishedAt, "%c") << ".\n";
}

// Instruction-related methods
void Process::setInstruction(int totalCount) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string instruction = "Instruction from " + processName;
    for (int i = 0; i < totalCount; i++) {
        instructionList.push_back(instruction);
    }
    std::cout << "Process ID=" << pid << ": Initialized with " << totalCount << " instructions.\n";
}

bool Process::isDone() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentInstructionLine >= instructionList.size();
}

void Process::execute() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!isDone()) {
        std::cout << "Process ID=" << pid << " executing instruction " << currentInstructionLine + 1
                  << "/" << instructionList.size() << ".\n";
        // Simulated workload
        for (size_t i = 0; i < 10000; i++) {}  // Simulating a delay
        currentInstructionLine++;
        if (isDone()) {
            std::time_t now = std::time(nullptr);
            setFinishedAt(std::localtime(&now));
            std::cout << "Process ID=" << pid << " has finished executing all instructions.\n";
        }
    } else {
        std::cout << "Process ID=" << pid << " is already completed.\n";
    }
}

void Process::setFrameIndices(const std::vector<size_t>& indices) {
    std::lock_guard<std::mutex> lock(mtx);
    frameIndices = indices;
}

std::vector<size_t> Process::getFrameIndices() const {
    std::lock_guard<std::mutex> lock(mtx);
    return frameIndices;
}
