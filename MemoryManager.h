#pragma once
#include <vector>
#include <string>

/**
 * MemoryBlock struct represents a block of memory with start and end addresses,
 * a flag to indicate if it is occupied, and the name of the process using it.
 */
struct MemoryBlock {
    int startAddress;      // Starting address of the memory block
    int endAddress;        // Ending address of the memory block
    bool isOccupied;       // Indicates if the block is currently in use
    std::string processName; // Name of the process occupying the block (empty if free)
};

/**
 * MemoryManager class simulates a simple first-fit memory allocator.
 * - Manages memory allocation, deallocation, and fragmentation calculation.
 * - Creates periodic snapshots of memory to track allocation status.
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
     * Allocates memory for a process using the first-fit strategy.
     * @param processName Name of the process requesting memory.
     * @return True if allocation was successful, False if insufficient memory.
     */
    bool allocateMemory(const std::string& processName);

    /**
     * Deallocates memory assigned to a given process.
     * @param processName Name of the process to free memory for.
     */
    void deallocateMemory(const std::string& processName);

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



    // Method to get memory utilization percentage
    double getMemoryUtilization() const;

    // Method to get external fragmentation in bytes
    double getExternalFragmentation() const;

    // Method to get the total memory size
    double getTotalMemory() const;

    // Method to get the free memory size
    double getFreeMemory() const;

private:
    int maxMemory;          // Total memory available in the system
    int processMemory;      // Fixed memory required per process
    std::vector<MemoryBlock> memory; // Vector representing memory blocks
    double totalMemory;  // Total memory size
    double freeMemory;   // Free memory available
    double minBlockSizeForAllocation;
    std::vector<double> freeBlocks;  // Vector storing sizes of free memory blocks


    /**
     * Merges adjacent free blocks to reduce fragmentation.
     * Called automatically after memory deallocation.
     */
    void mergeFreeBlocks();
};
