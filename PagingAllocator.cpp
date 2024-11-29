#include "PagingAllocator.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <limits> // For SIZE_MAX
#include <fstream> // For snapshot saving
#include <filesystem> // For directory checks (C++17)

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096 // 4KB page size
#endif

// Default Constructor
PagingAllocator::PagingAllocator()
    : maxMemorySize(0), numFrames(0), flatMemoryAllocated(0), memoryMode("flat") {}

// Parameterized Constructor
PagingAllocator::PagingAllocator(size_t maxMemorySize, const std::string& memoryMode)
    : maxMemorySize(maxMemorySize), numFrames(maxMemorySize / PAGE_SIZE),
    flatMemoryAllocated(0), memoryMode(memoryMode) {
    if (numFrames == 0) {
        std::cerr << "Error: maxMemorySize is too small for even one frame.\n";
        return;
    }

    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i); // Populate free frames
    }

    std::cout << "PagingAllocator Initialized with " << numFrames
        << " frames (" << maxMemorySize << " bytes total) in " << memoryMode << " mode.\n";
}

// Set Backing Store Directory
void PagingAllocator::setBackingStoreDirectory(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        if (!std::filesystem::create_directory(directory)) {
            std::cerr << "Error: Unable to create backing store directory: " << directory << "\n";
            return;
        }
    }
    backingStoreDirectory = directory;
    std::cout << "Backing store directory set to: " << directory << "\n";
}

// Generate Backing Store Filename
std::string PagingAllocator::generateBackingStoreFilename(size_t processId) const {
    return backingStoreDirectory + "/process_" + std::to_string(processId) + ".txt";
}

// Allocate Memory
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
        if (!evictOldestPage()) {
            std::cerr << "Paging: Allocation failed for process "
                << process->getProcessID() << ".\n";
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

// Deallocate Memory
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

    backingStore.remove_if([processId](const auto& page) {
        return page.second == std::to_string(processId);
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

// Evict Oldest Page
bool PagingAllocator::evictOldestPage() {
    if (frameMap.empty()) {
        std::cerr << "Paging: No frames to evict.\n";
        return false;
    }

    auto oldestFrame = frameMap.begin();
    std::string filename = generateBackingStoreFilename(oldestFrame->second);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to save evicted page to backing store.\n";
        return false;
    }

    file << "Frame " << oldestFrame->first << " -> Process " << oldestFrame->second << "\n";
    file.close();

    freeFrameList.push_back(oldestFrame->first);
    frameMap.erase(oldestFrame);

    std::cout << "Paging: Evicted Frame " << oldestFrame->first
        << " to " << filename << ".\n";
    return true;
}

// Restore Page from Backing Store
bool PagingAllocator::restorePageFromBackingStore(size_t processId) {
    std::string filename = generateBackingStoreFilename(processId);
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Paging: No backing store file for process " << processId << ".\n";
        return false;
    }

    std::cout << "Paging: Restored data for Process " << processId
        << " from " << filename << ".\n";
    file.close();
    return true;
}

// Visualize Memory
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

// Calculate External Fragmentation
size_t PagingAllocator::calculateExternalFragmentation() const {
    return freeFrameList.size() * PAGE_SIZE;
}

void PagingAllocator::saveProcessToBackingStore(Process* process) {
    if (!process) {
        std::cerr << "Error: Cannot save a null process to the backing store.\n";
        return;
    }

    std::string filename = generateBackingStoreFilename(process->getProcessID());
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to save process to backing store.\n";
        return;
    }

    file << "Process ID: " << process->getProcessID() << "\n";
    file << "Process Name: " << process->getName() << "\n";
    file << "Command Counter: " << process->getCommandCounter() << "\n"; // Ensure Command Counter is accessible
    file << "Number of Pages: " << process->getNumPages() << "\n";
    file << "Memory Size: " << process->getMemorySize() << "\n";

    file.close();
    std::cout << "Process " << process->getName() << " saved to " << filename << ".\n";
}


// Memory Map (Retained from Original Implementation)
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

// Memory Utilization (Retained from Original Implementation)
float PagingAllocator::getMemoryUtilization() const {
    size_t totalFrames = numFrames;
    size_t usedFrames = frameMap.size();
    return static_cast<float>(usedFrames) / totalFrames * 100.0f;
}

// External Fragmentation (Retained from Original Implementation)
size_t PagingAllocator::calculateExternalFragmentation() const {
    size_t fragmentedMemory = 0;
    size_t freeFrameCount = freeFrameList.size();
    size_t totalFrames = numFrames;

    fragmentedMemory = (totalFrames - freeFrameCount) * PAGE_SIZE;
    return fragmentedMemory;
}

// Deallocate Frames (Retained from Original Implementation)
void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex, const std::vector<size_t>& pageSizes) {
    if (numFrames == 0 || frameIndex >= numFrames) {
        std::cerr << "Error: Invalid number of frames or frame index for deallocation.\n";
        return;
    }

    for (size_t i = 0; i < numFrames; ++i) {
        if (frameIndex + i < numFrames) {
            freeFrameList.push_back(frameIndex + i); // Reclaim the frame
            frameMap.erase(frameIndex + i); // Remove from frameMap
            std::cout << "Deallocated Frame " << frameIndex + i << "\n";
        }
    }
}

// Getter for Frame Map (Retained from Original Implementation)
const std::unordered_map<size_t, size_t>& PagingAllocator::getFrameMap() const {
    return frameMap;
}
