#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream> // For file operations
#include "Process.h" // Assuming Process is a class that provides getProcessID(), getMemorySize(), and getNumPages() methods

// Define the page size as 4KB if not already defined
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

class PagingAllocator {
public:
    // Default Constructor
    PagingAllocator(); // Initializes with default values (no memory allocated)

    // Constructor with specified memory size
    explicit PagingAllocator(size_t maxMemorySize);

    // Allocation and deallocation
    void* allocate(Process* process); // Allocates memory for a process
    void deallocate(Process* process); // Deallocates memory for a process

    // Visualization
    void visualizeMemory() const; // Prints the memory allocation map
    std::string getMemoryMap() const; // Returns a string representation of the memory map

    // Memory statistics
    float getMemoryUtilization() const; // Returns memory utilization as a percentage
    size_t calculateExternalFragmentation() const; // Calculates external fragmentation in bytes

    // Internal utility functions
    size_t allocateFrames(size_t numFrames, size_t processId, const std::vector<size_t>& pageSizes); // Allocates frames for a process
    void deallocateFrames(size_t numFrames, size_t frameIndex, const std::vector<size_t>& pageSizes); // Deallocates frames for a process

    // Snapshot
    void saveMemorySnapshot(size_t snapshotId) const; // Saves the current memory state to a file

private:
    size_t maxMemorySize; // Total memory size in bytes
    size_t numFrames; // Total number of frames (maxMemorySize / PAGE_SIZE)
    std::unordered_map<size_t, size_t> frameMap; // Maps frame index to process ID
    std::vector<size_t> freeFrameList; // List of free frame indices
};
