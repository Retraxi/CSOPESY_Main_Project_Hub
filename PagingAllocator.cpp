#include "PagingAllocator.h"
#include <iostream>

PagingAllocator::PagingAllocator(size_t totalFrames) : totalFrames(totalFrames) {
    for (size_t i = 0; i < totalFrames; ++i) {
        freeFrameList.push_back(i); // Initialize all frames as free
    }
}

void* PagingAllocator::allocateToProcess(size_t numFrames, Process& process) {
    std::lock_guard<std::mutex> lock(mtx);

    if (numFrames > freeFrameList.size()) {
        std::cerr << "Allocation failed: Not enough free frames.\n";
        return nullptr; // Allocation failed
    }

    std::vector<size_t> allocatedFrames;
    for (size_t i = 0; i < numFrames; ++i) {
        size_t frame = freeFrameList.back();
        freeFrameList.pop_back();
        frameMap[frame] = process.getProcessID();
        allocatedFrames.push_back(frame);
    }

    process.setAllocatedFrames(allocatedFrames);
    std::cout << "Allocated " << numFrames << " frames to Process " 
              << process.getProcessID() << ".\n";
    return static_cast<void*>(&allocatedFrames); // Return a pointer to the allocation
}

void PagingAllocator::deallocate(int processID) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = frameMap.begin();
    while (it != frameMap.end()) {
        if (it->second == processID) {
            freeFrameList.push_back(it->first); // Free the frame
            it = frameMap.erase(it);           // Remove from map
        } else {
            ++it;
        }
    }

    std::cout << "Deallocated frames for Process " << processID << ".\n";
}
