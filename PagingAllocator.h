#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream> // For file operations
#include <list>    // For backing store representation
#include "Process.h" // Assuming Process is a class that provides getProcessID(), getMemorySize(), and getNumPages() methods
#include <iostream>

// Define the page size as 4KB if not already defined
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

class PagingAllocator {
public:
    // Default Constructor
    PagingAllocator(); // Initializes with default values (no memory allocated)

    void setBackingStoreDirectory(const std::string& directory); // Sets the backing store directory


    PagingAllocator(size_t maxMemorySize);

    // Constructor with specified memory size and mode
    explicit PagingAllocator(size_t maxMemorySize, const std::string& memoryMode);

    // Allocation and deallocation
    void* allocate(Process* process); // Allocates memory for a process
    void deallocate(Process* process); // Deallocates memory for a process

    // Visualization
    void visualizeMemory() const; // Prints the memory allocation map
    std::string getMemoryMap() const; // Returns a string representation of the memory map
    void visualizeBackingStore() const; // Visualizes the content of the backing store

    // Memory statistics
    float getMemoryUtilization() const; // Returns memory utilization as a percentage
    size_t calculateExternalFragmentation() const; // Calculates external fragmentation in bytes

    // Backing Store Operations
    bool evictOldestPage(); // Evicts the oldest page to the backing store
    bool restorePageFromBackingStore(size_t processId); // Restores a page from the backing store for a given process
    void saveProcessToBackingStore(Process* process); // NEW: Saves process data to the backing store
    Process* loadProcessFromBackingStore(size_t processId); // NEW: Loads process data from the backing store

    // Internal utility functions
    size_t allocateFrames(size_t numFrames, size_t processId, const std::vector<size_t>& pageSizes); // Allocates frames for a process
    void deallocateFrames(size_t numFrames, size_t frameIndex, const std::vector<size_t>& pageSizes); // Deallocates frames for a process

    // Snapshot
    void saveMemorySnapshot(size_t snapshotId) const; // Saves the current memory state to a file

    // Getter for frameMap (used for debugging and backing store operations)
    const std::unordered_map<size_t, size_t>& getFrameMap() const;

private:
    size_t maxMemorySize; // Total memory size in bytes
    size_t numFrames; // Total number of frames (maxMemorySize / PAGE_SIZE)
    std::unordered_map<size_t, size_t> frameMap; // Maps frame index to process ID
    std::vector<size_t> freeFrameList; // List of free frame indices

    std::string backingStoreDirectory; // Directory for backing store files


    // Backing Store
    std::list<std::pair<size_t, std::string>> backingStore; // Modified: Stores evicted processes as (processId, filename)

    // Helper function for backing store file management
    std::string generateBackingStoreFilename(size_t processId) const; // NEW: Generates a filename for a process in the backing store

    // Flat memory mode attributes
    std::string memoryMode; // Memory mode: "flat" or "paged"
    size_t flatMemoryAllocated; // Tracks memory allocated in flat mode

    // Flat memory management utilities
    bool allocateFlatMemory(Process* process); // Handles flat memory allocation
    void deallocateFlatMemory(Process* process); // Handles flat memory deallocation
};