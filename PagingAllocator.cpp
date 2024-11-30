#include "PagingAllocator.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

// Constructor
PagingAllocator::PagingAllocator(size_t maxMemory) : maxMemory(maxMemory), allocatedSize(0) {
    numFrames = maxMemory / PAGE_SIZE;
    freeFrameList.resize(numFrames);
    std::iota(freeFrameList.begin(), freeFrameList.end(), 0);  // Initialize free frame list
    // Initialize backing store
    backingStore.open(".pagefile", std::ios::binary | std::ios::trunc);
    backingStore.close();
}

// Destructor
PagingAllocator::~PagingAllocator() {
    if (backingStore.is_open()) {
        backingStore.close();
    }
}

// Allocate memory
void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
    size_t numFramesNeeded = process->getMemorySize() / PAGE_SIZE;
    if (numFramesNeeded <= freeFrameList.size() && process->getFrameIndices().empty()) {
        processOrder.push(process);  // Track process for potential eviction
        size_t frameIndex = allocateFrames(numFramesNeeded, process->getProcessID());
        return reinterpret_cast<void*>(frameIndex);
    } else {
        if (freeFrameList.size() < numFramesNeeded) {
            evictOldestProcess();
            return allocate(process);  // Retry allocation after eviction
        }
    }
    return nullptr;  // Allocation failed if no frames available
}

// Deallocate memory
void PagingAllocator::deallocate(std::shared_ptr<Process> process) {
    deallocateFrames(process);
}

// Allocate frames to process
size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processID) {
    size_t frameIndex = freeFrameList.back();
    freeFrameList.pop_back();
    for (size_t i = 0; i < numFrames; i++) {
        frameMap[frameIndex + i] = processID;
        pagesAllocated++;
    }
    return frameIndex;
}

// Deallocate frames
void PagingAllocator::deallocateFrames(std::shared_ptr<Process> process) {
    auto& frames = process->getFrameIndices();
    for (auto frame : frames) {
        frameMap.erase(frame);
        freeFrameList.push_back(frame);
        pagesDeallocated++;
    }
    frames.clear();
}

// Evict the oldest process not currently running
void PagingAllocator::evictOldestProcess() {
    if (!processOrder.empty()) {
        auto oldestProcess = processOrder.front();
        if (oldestProcess->getCoreID() == -1) {  // Check if process is not running
            deallocate(oldestProcess);
            processOrder.pop();
        }
    }
}

// Visualize the current state of memory
void PagingAllocator::visualizeMemory() {
    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
        auto it = frameMap.find(frameIndex);
        if (it != frameMap.end()) {
            std::cout << "Frame " << frameIndex << " -> Process " << it->second << std::endl;
        } else {
            std::cout << "Frame " << frameIndex << " Free" << std::endl;
        }
    }
}

// VMStat: Report virtual memory statistics
void PagingAllocator::vmstat() const {
    std::cout << "VM Statistics:\n"
              << "Total Memory: " << maxMemory << " bytes\n"
              << "Pages Allocated: " << pagesAllocated << "\n"
              << "Pages Deallocated: " << pagesDeallocated << "\n"
              << "Allocated Memory: " << pagesAllocated * PAGE_SIZE << " bytes\n"
              << "Free Memory: " << freeFrameList.size() * PAGE_SIZE << " bytes\n";
}
