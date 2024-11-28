#pragma once
#include "PagingAllocator.h"
#include "Process.h"
#include <queue>
#include <vector>
#include <unordered_map>

class PagingScheduler {
public:
    PagingScheduler(size_t totalFrames);
    ~PagingScheduler() = default;

    void addProcess(Process* process);
    void run();
    void deallocateProcess(int processID);

private:
    PagingAllocator allocator;
    std::queue<Process*> readyQueue; // Queue of ready processes
    std::vector<Process*> completedProcesses; // Track completed processes
    std::vector<Process*> backingStore; // Backing store for processes

    // Helper function to move processes from backing store to ready queue
    void checkBackingStore();
};
