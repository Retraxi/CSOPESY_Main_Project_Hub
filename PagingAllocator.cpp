#include "PagingAllocator.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <limits> // For SIZE_MAX
#include <fstream> // For snapshot saving

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096 // 4KB page size
#endif

// Default Constructor
PagingAllocator::PagingAllocator() : maxMemorySize(0), numFrames(0) {}

// Parameterized Constructor
PagingAllocator::PagingAllocator(size_t maxMemorySize)
    : maxMemorySize(maxMemorySize), numFrames(maxMemorySize / PAGE_SIZE) {
    if (numFrames == 0) {
        std::cerr << "Error: maxMemorySize is too small for even one frame.\n";
        return;
    }

    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i); // Populate free frames
    }

    std::cout << "PagingAllocator Initialized with " << numFrames
        << " frames (" << maxMemorySize << " bytes total).\n";
    std::cout << "Initial Free Frames: " << freeFrameList.size() << "\n";
}

// Memory Allocation
void* PagingAllocator::allocate(Process* process) {
    if (!process) {
        std::cerr << "Error: Attempt to allocate memory for a null process.\n";
        return nullptr;
    }

    size_t numFramesNeeded = process->getNumPages();
    std::cout << "Allocating for Process " << process->getProcessID()
        << " (Frames Needed: " << numFramesNeeded
        << ", Free Frames: " << freeFrameList.size() << ")\n";

    if (numFramesNeeded > freeFrameList.size()) {
        std::cerr << "Paging: Allocation failed for process "
            << process->getProcessID()
            << " (Frames Needed: " << numFramesNeeded
            << ", Free Frames: " << freeFrameList.size() << ")\n";
        return nullptr;
    }

    size_t startingFrame = allocateFrames(numFramesNeeded, process->getProcessID(), {});
    if (startingFrame == SIZE_MAX) {
        std::cerr << "Paging: Unable to allocate frames for process "
            << process->getProcessID() << ".\n";
        return nullptr;
    }

    std::cout << "Paging: Successfully allocated " << numFramesNeeded
        << " frames for process " << process->getProcessID() << ".\n";
    return reinterpret_cast<void*>(startingFrame);
}

// Memory Deallocation
void PagingAllocator::deallocate(Process* process) {
    if (!process) {
        std::cerr << "Error: Attempt to deallocate memory for a null process.\n";
        return;
    }

    size_t processId = process->getProcessID();
    bool deallocated = false;

    std::cout << "Deallocating Process " << processId << "...\n";

    for (auto it = frameMap.begin(); it != frameMap.end();) {
        if (it->second == processId) {
            std::cout << "Deallocating Frame " << it->first << " from Process " << processId << ".\n";
            freeFrameList.push_back(it->first);
            it = frameMap.erase(it);
            deallocated = true;
        }
        else {
            ++it;
        }
    }

    if (deallocated) {
        std::cout << "Paging: Successfully deallocated memory for process "
            << processId << ".\n";
    }
    else {
        std::cerr << "Paging: No memory was allocated to process "
            << processId << ".\n";
    }

    std::cout << "Free Frames After Deallocation: " << freeFrameList.size() << "\n";
}

// Memory Visualization
void PagingAllocator::visualizeMemory() const {
    std::cout << "Memory Visualization:\n";
    for (size_t i = 0; i < numFrames; ++i) {
        auto it = frameMap.find(i);
        if (it != frameMap.end()) {
            std::cout << "Frame " << i << " -> Process " << it->second << "\n";
        }
        else {
            std::cout << "Frame " << i << " -> Free\n";
        }
    }
}

// Get Memory Map as String
std::string PagingAllocator::getMemoryMap() const {
    std::ostringstream memoryMap;
    memoryMap << "Frame Map:\n";
    for (const auto& frame : frameMap) {
        memoryMap << "Frame " << frame.first << ": Process ID " << frame.second << "\n";
    }
    memoryMap << "Free Frames: " << freeFrameList.size() << "\n";
    return memoryMap.str();
}

// Allocate Frames
size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processId, const std::vector<size_t>& pageSizes) {
    if (numFrames > freeFrameList.size()) {
        std::cerr << "Error: Not enough free frames to allocate.\n";
        return SIZE_MAX; // Not enough free frames
    }

    size_t startingFrame = freeFrameList.back();
    for (size_t i = 0; i < numFrames; ++i) {
        if (freeFrameList.empty()) {
            std::cerr << "Error: No free frames available during allocation.\n";
            return SIZE_MAX;
        }

        size_t frame = freeFrameList.back();
        freeFrameList.pop_back();
        frameMap[frame] = processId;

        std::cout << "Allocated Frame " << frame << " to Process " << processId << ".\n";
    }
    return startingFrame;
}

// Deallocate Frames
void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex, const std::vector<size_t>& pageSizes) {
    for (size_t i = 0; i < numFrames; ++i) {
        if (frameIndex + i >= numFrames) {
            std::cerr << "Error: Attempt to deallocate an invalid frame index.\n";
            return;
        }

        frameMap.erase(frameIndex + i);
        freeFrameList.push_back(frameIndex + i);
        std::cout << "Deallocated Frame " << (frameIndex + i) << ".\n";
    }
}

// Get Memory Utilization
float PagingAllocator::getMemoryUtilization() const {
    size_t usedFrames = frameMap.size();
    return static_cast<float>(usedFrames) / numFrames * 100.0f;
}

// Calculate External Fragmentation
size_t PagingAllocator::calculateExternalFragmentation() const {
    return freeFrameList.size() * PAGE_SIZE; // Free frames multiplied by page size
}

// Save Memory Snapshot
// Save Memory Snapshot
void PagingAllocator::saveMemorySnapshot(size_t snapshotId) const {
    std::ofstream snapshotFile("memory_snapshot_" + std::to_string(snapshotId) + ".txt");
    if (!snapshotFile.is_open()) {
        std::cerr << "Error: Unable to save memory snapshot.\n";
        return;
    }

    snapshotFile << "Memory Snapshot #" << snapshotId << "\n";
    snapshotFile << "Frame Map:\n";

    // Include detailed frame information
    for (size_t i = 0; i < numFrames; ++i) {
        auto it = frameMap.find(i);
        if (it != frameMap.end()) {
            snapshotFile << "Frame " << i << " -> Process " << it->second << "\n";
        }
        else {
            snapshotFile << "Frame " << i << " -> Free\n";
        }
    }

    snapshotFile << "Free Frames: " << freeFrameList.size() << "\n";
    snapshotFile.close();

    std::cout << "Paging: Memory snapshot #" << snapshotId << " saved successfully.\n";
}
