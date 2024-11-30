#pragma once
#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <ctime>

class Process {
public:
    Process(int pid, std::string name, size_t memorySize = 0);
    ~Process() = default;

    // Thread-safe getters
    std::string getName() const { std::lock_guard<std::mutex> lock(mtx); return processName; }
    int getProcessID() const { return pid; }
    int getCoreID() const { std::lock_guard<std::mutex> lock(mtx); return coreID; }
    size_t getMemorySize() const { return memorySize; }
    std::vector<size_t> getFrameIndices() const { std::lock_guard<std::mutex> lock(mtx); return frameIndices; }

    // Time-related methods
    std::tm* getCreatedAt() const { return createdAt; }
    std::tm* getFinishedAt() const { return finishedAt; }
    void setFinishedAt(const std::tm& time);

    // Setters and modifiers
    void setCoreID(int coreID);
    void setMemorySize(size_t memorySize);
    void setFrameIndices(const std::vector<size_t>& indices);

    // Execution-related methods
    void execute();
    bool isDone();

private:
    mutable std::mutex mtx;

    int pid;
    std::string processName;
    std::vector<std::string> instructionList;
    int currentInstructionLine = 0;
    int coreID = -1;

    std::tm* createdAt;
    std::tm* finishedAt = nullptr;
    size_t memorySize;
    std::vector<size_t> frameIndices;
};

#endif
