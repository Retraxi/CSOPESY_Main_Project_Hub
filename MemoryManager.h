#pragma once
#include <vector>
#include <string>
#include <list> // For backing store implementation
#include <queue>
#include <unordered_map>
#include "PagingAllocator.h" // Include the PagingAllocator

/**
 * MemoryBlock struct represents a block of memory with start and end addresses,
 * a flag to indicate if it is occupied, and the name of the process using it.
 */
struct MemoryBlock {
    int startAddress;        // Starting address of the memory block
    int endAddress;          // Ending address of the memory block
    bool isOccupied;         // Indicates if the block is currently in use
    std::string processName; // Name of the process occupying the block (empty if free)

    // Equality operator to compare MemoryBlock objects
    bool operator==(const MemoryBlock& other) const {
        return (startAddress == other.startAddress) &&
            (endAddress == other.endAddress) &&
            (isOccupied == other.isOccupied) &&
            (processName == other.processName);
    }
};

/**
 * MemoryManager class simulates a simple memory management system.
 * - Supports both flat memory allocation and paging allocation.
 * - Manages memory allocation, deallocation, fragmentation, and visualization.
 * - Includes a backing store to handle swapping out processes/pages.
 */
class MemoryManager {
public:
    /**
     * Constructor for MemoryManager.
     * @param maxMemory Total memory available (in bytes).
     * @param processMemory Memory required per process (in bytes).
     */
    MemoryManager(int maxMemory, int processMemory);

    /**
     * Allocates memory for a process using the current allocation mode.
     * @param processName Name of the process requesting memory.
     * @return True if allocation was successful, False if insufficient memory.
     */
    bool allocateMemory(const std::string& processName);

    /**
     * Allocates memory in paging mode with a specific size.
     * @param processName Name of the process requesting memory.
     * @param size Memory size requested (in bytes).
     * @return True if allocation was successful, False if insufficient memory.
     */
    bool allocateMemory(const std::string& processName, size_t size);

    /**
     * Deallocates memory assigned to a given process.
     * @param processName Name of the process to free memory for.
     */
    void deallocateMemory(const std::string& processName);

    /**
     * Restores a process from the backing store to main memory.
     * @param processName Name of the process to restore.
     * @return True if the process was restored successfully, False otherwise.
     */
    bool restoreProcessFromBackingStore(const std::string& processName);

    /**
     * Calculates the total external fragmentation (unused memory between blocks).
     * @return The amount of fragmented memory (in bytes).
     */
    int calculateExternalFragmentation() const;

    /**
     * Saves a memory snapshot to a file, showing allocation status and fragmentation.
     * @param quantumCycle Current quantum cycle for snapshot identification.
     */
    void saveMemorySnapshot(int quantumCycle) const;

    /**
     * Visualizes the current memory allocation.
     * - In flat mode, displays the blocks of memory.
     * - In paging mode, displays the frame allocation.
     */
    void visualizeMemory() const;

    /**
     * Switches to flat memory allocation mode.
     */
    void switchToFlat();

    /**
     * Switches to paging memory allocation mode.
     */
    void switchToPaging();

    // Method to get memory utilization percentage
    double getMemoryUtilization() const;

    // Method to get external fragmentation in bytes
    double getExternalFragmentation() const;

    // Method to get the total memory size
    double getTotalMemory() const;

    // Method to get the free memory size
    double getFreeMemory() const;

private:
    int maxMemory;                   // Total memory available in the system
    int processMemory;               // Fixed memory required per process
    std::vector<MemoryBlock> memory; // Vector representing memory blocks
    double totalMemory;              // Total memory size
    double freeMemory;               // Free memory available
    double minBlockSizeForAllocation;
    std::vector<double> freeBlocks;  // Vector storing sizes of free memory blocks

    bool isPaging;                   // Indicates the current memory allocation mode
    PagingAllocator pagingAllocator; // Paging allocator instance

    // Backing store for evicted processes (flat memory)
    std::list<MemoryBlock> flatBackingStore; // For flat memory eviction

    // Backing store for evicted processes (paging memory)
    std::unordered_map<std::string, Process> pageBackingStore; // For paging memory eviction

    std::queue<std::string> evictionQueue; // Queue for managing eviction order

    /**
     * Merges adjacent free blocks to reduce fragmentation.
     * Called automatically after memory deallocation.
     */
    void mergeFreeBlocks();

    /**
     * Evicts the oldest process to the backing store (flat memory).
     * Frees memory to allow new processes to be allocated.
     * @return True if a process was evicted successfully, False otherwise.
     */
    void evictOldestProcess();

    /**
     * Evicts the oldest page to the backing store (paging memory).
     * Frees frames to allow new pages to be allocated.
     * @return True if a page was evicted successfully, False otherwise.
     */
    void evictOldestPage();
};
