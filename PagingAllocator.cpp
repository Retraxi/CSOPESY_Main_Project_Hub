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
        // Evict oldest page to backing store if needed
        if (!evictOldestPage()) {
            std::cerr << "Paging: Allocation failed for process "
                << process->getProcessID()
                << " (Frames Needed: " << numFramesNeeded
                << ", Free Frames: " << freeFrameList.size() << "). No pages could be evicted.\n";
            return nullptr;
        }
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

    // Remove from backing store if present
    backingStore.remove_if([processId](const auto& page) {
        return page.second == processId;
        });

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

// Backing Store: Evict Oldest Page
bool PagingAllocator::evictOldestPage() {
    if (frameMap.empty()) {
        std::cerr << "Paging: No frames to evict.\n";
        return false;
    }

    // Evict the first (oldest) frame
    auto oldestFrame = frameMap.begin();
    backingStore.push_back({ oldestFrame->first, oldestFrame->second });
    std::cout << "Paging: Evicted Frame " << oldestFrame->first << " (Process "
        << oldestFrame->second << ") to backing store.\n";

    freeFrameList.push_back(oldestFrame->first);
    frameMap.erase(oldestFrame);

    return true;
}

// Backing Store: Restore Page
bool PagingAllocator::restorePageFromBackingStore(size_t processId) {
    for (auto it = backingStore.begin(); it != backingStore.end(); ++it) {
        if (it->second == processId) {
            size_t frame = it->first;

            if (freeFrameList.empty()) {
                std::cerr << "Paging: No free frames to restore from backing store.\n";
                return false;
            }

            frameMap[frame] = processId;
            freeFrameList.pop_back();
            std::cout << "Paging: Restored Frame " << frame << " for Process " << processId << " from backing store.\n";

            backingStore.erase(it);
            return true;
        }
    }

    std::cerr << "Paging: No pages for Process " << processId << " found in backing store.\n";
    return false;
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
    visualizeBackingStore();
}

// Visualize Backing Store
void PagingAllocator::visualizeBackingStore() const {
    std::cout << "Backing Store Visualization:\n";
    for (const auto& page : backingStore) {
        std::cout << "Frame " << page.first << " -> Process " << page.second << "\n";
    }
}

// Allocate Frames
size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processId, const std::vector<size_t>& pageSizes) {
    if (numFrames > freeFrameList.size()) {
        std::cerr << "Error: Not enough free frames to allocate.\n";
        return SIZE_MAX; // Not enough free frames
    }

    size_t startingFrame = freeFrameList.back();
    for (size_t i = 0; i < numFrames; ++i) {
        size_t frame = freeFrameList.back();
        freeFrameList.pop_back();
        frameMap[frame] = processId;
        std::cout << "Allocated Frame " << frame << " to Process " << processId << ".\n";
    }
    return startingFrame;
}

// Save Memory Snapshot
void PagingAllocator::saveMemorySnapshot(size_t snapshotId) const {
    std::ofstream snapshotFile("memory_snapshot_" + std::to_string(snapshotId) + ".txt");
    if (!snapshotFile.is_open()) {
        std::cerr << "Error: Unable to save memory snapshot.\n";
        return;
    }

    snapshotFile << "Memory Snapshot #" << snapshotId << "\n";
    snapshotFile << "Frame Map:\n";

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
    snapshotFile << "Backing Store:\n";

    for (const auto& page : backingStore) {
        snapshotFile << "Frame " << page.first << " -> Process " << page.second << "\n";
    }

    snapshotFile.close();
    std::cout << "Paging: Memory snapshot #" << snapshotId << " saved successfully.\n";
}

// Memory Map
std::string PagingAllocator::getMemoryMap() const {
    std::stringstream mapStream;
    for (size_t i = 0; i < numFrames; ++i) {
        auto it = frameMap.find(i);
        if (it != frameMap.end()) {
            mapStream << "Frame " << i << " -> Process " << it->second << "\n";
        }
        else {
            mapStream << "Frame " << i << " -> Free\n";
        }
    }
    return mapStream.str();
}

// Memory Utilization
float PagingAllocator::getMemoryUtilization() const {
    size_t totalFrames = numFrames;
    size_t usedFrames = frameMap.size();
    return static_cast<float>(usedFrames) / totalFrames * 100.0f;
}

// External Fragmentation
size_t PagingAllocator::calculateExternalFragmentation() const {
    size_t fragmentedMemory = 0;
    size_t freeFrameCount = freeFrameList.size();
    size_t totalFrames = numFrames;

    // External fragmentation occurs when free frames are scattered,
    // but for simplicity, we're just calculating how much memory is unused.
    fragmentedMemory = (totalFrames - freeFrameCount) * PAGE_SIZE;
    return fragmentedMemory;
}

// Deallocate Frames
void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex, const std::vector<size_t>& pageSizes) {
    // This function assumes we're deallocating frames that were previously allocated.
    if (numFrames == 0 || frameIndex >= numFrames) {
        std::cerr << "Error: Invalid number of frames or frame index for deallocation.\n";
        return;
    }

    for (size_t i = 0; i < numFrames; ++i) {
        if (frameIndex + i < numFrames) {
            freeFrameList.push_back(frameIndex + i);  // Reclaim the frame
            frameMap.erase(frameIndex + i);  // Remove from frameMap
            std::cout << "Deallocated Frame " << frameIndex + i << "\n";
        }
    }
}

// Getter for Frame Map (used for debugging and backing store operations)
const std::unordered_map<size_t, size_t>& PagingAllocator::getFrameMap() const {
    return frameMap;
}