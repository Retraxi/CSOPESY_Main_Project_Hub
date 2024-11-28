#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>
#include "Process.h"

class PagingAllocator {
public:
    PagingAllocator(size_t totalFrames);
    ~PagingAllocator() = default;

    void* allocateToProcess(size_t numFrames, Process& process);
    void deallocate(int processID);

private:
    size_t totalFrames;                   // Total frames available
    std::vector<size_t> freeFrameList;    // List of free frames
    std::unordered_map<size_t, int> frameMap; // Maps frames to process IDs
    std::mutex mtx;                       // For thread safety
};
