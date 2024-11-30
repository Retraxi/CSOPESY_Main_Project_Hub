#include "PagingAllocator.h"
#include <iostream>
#include <algorithm> // For std::find

PagingAllocator::PagingAllocator(size_t maxMemory, size_t memPerFrame) {
    if (maxMemory == 0 || memPerFrame == 0) {
        std::cerr << "Error: maxMemory or memPerFrame cannot be zero." << std::endl;
        throw std::invalid_argument("Invalid memory or frame size.");
    }

    this->maxMemory = maxMemory;
    this->memPerFrame = memPerFrame;

    numFrames = maxMemory / memPerFrame;
    if (numFrames == 0) {
        std::cerr << "Error: No memory frames available with the given configuration." << std::endl;
        throw std::runtime_error("No memory frames available.");
    }

    // Initialize free frames
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i);
    }

    allocatedSize = 0;
    pagesAllocated = 0;
    pagesDeallocated = 0;

    std::cout << "PagingAllocator initialized with " << numFrames
        << " frames (" << memPerFrame << " bytes per frame)." << std::endl;
}

PagingAllocator::~PagingAllocator() {
    freeFrameList.clear();
    frameMap.clear();
    std::cout << "PagingAllocator destroyed. All memory has been cleared." << std::endl;
}

void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
    if (!process) {
        std::cerr << "Error: Process is null." << std::endl;
        return nullptr;
    }

    size_t processMemorySize = process->getMemorySize();
    if (processMemorySize == 0) {
        std::cerr << "Error: Process memory size is zero." << std::endl;
        return nullptr;
    }

    // Calculate the number of frames needed
    size_t numFramesNeeded = (processMemorySize + memPerFrame - 1) / memPerFrame;

    std::cout << "Process " << process->getProcessID()
        << " requesting " << processMemorySize
        << " bytes (" << numFramesNeeded << " frames)." << std::endl;

    if (numFramesNeeded > freeFrameList.size()) {
        std::cerr << "Error: Insufficient memory frames available for process "
            << process->getProcessID() << ". Requested: " << numFramesNeeded
            << ", Available: " << freeFrameList.size() << "." << std::endl;
        return nullptr;
    }

    // Allocate frames
    std::vector<size_t> allocatedFrames;
    for (size_t i = 0; i < numFramesNeeded; ++i) {
        size_t frameIndex = freeFrameList.back();
        freeFrameList.pop_back(); // Remove from free list
        allocatedFrames.push_back(frameIndex);
        frameMap[frameIndex] = process->getProcessID(); // Map frame to process
    }

    process->setFrameIndices(allocatedFrames); // Save allocated frames to the process
    std::cout << "Allocated " << numFramesNeeded << " frames to process " << process->getProcessID()
        << ". Free frames remaining: " << freeFrameList.size() << "." << std::endl;

    return reinterpret_cast<void*>(allocatedFrames.front());
}

void PagingAllocator::deallocate(std::shared_ptr<Process> process) {
    if (!process) {
        std::cerr << "Error: Process is null. Cannot deallocate." << std::endl;
        return;
    }

    std::vector<size_t> frameIndices = process->getFrameIndices();
    if (frameIndices.empty()) {
        std::cerr << "Error: Process " << process->getProcessID() << " has no allocated frames to deallocate." << std::endl;
        return;
    }

    std::cout << "Deallocating frames for process " << process->getProcessID() << ": ";
    for (size_t frameIndex : frameIndices) {
        std::cout << frameIndex << " ";
    }
    std::cout << std::endl;

    // Iterate over the frame indices and return them to the free frame list
    for (size_t frameIndex : frameIndices) {
        auto it = frameMap.find(frameIndex);
        if (it != frameMap.end() && it->second == process->getProcessID()) {
            frameMap.erase(it);                   // Remove frame from the map
            if (std::find(freeFrameList.begin(), freeFrameList.end(), frameIndex) == freeFrameList.end()) {
                freeFrameList.push_back(frameIndex); // Add frame back to the free list
            }
            else {
                std::cerr << "Warning: Frame " << frameIndex << " is already in the free list." << std::endl;
            }
        }
        else {
            std::cerr << "Warning: Frame " << frameIndex
                << " is not allocated to process " << process->getProcessID()
                << " or is already deallocated." << std::endl;
        }
    }

    // Sort freeFrameList for consistent allocation order
    std::sort(freeFrameList.begin(), freeFrameList.end());

    // Clear frame indices in the process to indicate deallocation
    process->setFrameIndices({});
    std::cout << "Successfully deallocated frames for process " << process->getProcessID()
        << ". Free frames available: " << freeFrameList.size() << "/" << numFrames << "." << std::endl;
}

void PagingAllocator::visualizeMemory() {
    std::cout << "Memory Visualization:" << std::endl;
    for (size_t frameIndex = 0; frameIndex < numFrames; frameIndex++) {
        auto it = frameMap.find(frameIndex);

        if (it != frameMap.end()) {
            std::cout << "Frame " << frameIndex << " -> Process " << it->second << std::endl;
        }
        else {
            std::cout << "Frame " << frameIndex << " Free" << std::endl;
        }
    }
    std::cout << "Total Free Frames: " << freeFrameList.size() << "/" << numFrames << std::endl;
}
