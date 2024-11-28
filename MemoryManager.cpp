#include "MemoryManager.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <iostream>

/**
 * Initializes the MemoryManager with the maximum memory and process memory size.
 */
MemoryManager::MemoryManager(int maxMemory, int processMemory)
    : maxMemory(maxMemory), processMemory(processMemory),
    pagingAllocator(maxMemory), freeMemory(maxMemory),
    totalMemory(maxMemory), minBlockSizeForAllocation(1), isPaging(false) {

    if (maxMemory < processMemory) {
        std::cerr << "Error: Maximum memory is less than process memory requirement.\n";
        return;
    }

    memory.push_back({ 0, maxMemory - 1, false, "" }); // Initial free block for flat memory
    std::cout << "MemoryManager Initialized in Flat Mode: maxMemory=" << maxMemory
        << ", processMemory=" << processMemory << "\n";
}

/**
 * Allocates memory for a process using the current allocation mode.
 */
bool MemoryManager::allocateMemory(const std::string& processName, size_t size) {
    if (isPaging) {
        Process process(1, processName, size); // Dummy PID used here
        void* allocation = pagingAllocator.allocate(&process);
        if (allocation) {
            std::cout << "MemoryManager: Allocated " << size << " bytes for process " << processName << " in Paging mode.\n";
            return true;
        }
        else {
            std::cerr << "MemoryManager: Failed to allocate memory for process " << processName << " in Paging mode.\n";
            return false;
        }
    }
    else {
        return allocateMemory(processName); // Flat allocation
    }
}

/**
 * Allocates memory for a process using the first-fit strategy (flat mode).
 */
bool MemoryManager::allocateMemory(const std::string& processName) {
    for (auto& block : memory) {
        if (!block.isOccupied && (block.endAddress - block.startAddress + 1) >= processMemory) {
            int allocatedEnd = block.startAddress + processMemory - 1;

            // Update the block to reflect the allocated range
            block.endAddress = allocatedEnd;
            block.isOccupied = true;
            block.processName = processName;

            // Add a new free block for any remaining memory
            if (allocatedEnd + 1 <= block.endAddress) {
                memory.push_back({ allocatedEnd + 1, block.endAddress, false, "" });
            }

            freeMemory -= processMemory;
            std::cout << "MemoryManager: Allocated " << processMemory << " bytes for process " << processName << " in Flat mode.\n";
            return true;
        }
    }
    std::cerr << "MemoryManager: Failed to allocate memory for process " << processName << " in Flat mode.\n";
    return false;
}

/**
 * Deallocates memory assigned to a given process.
 */
void MemoryManager::deallocateMemory(const std::string& processName) {
    if (isPaging) {
        Process process(1, processName, 0); // Dummy PID and size
        pagingAllocator.deallocate(&process);
    }
    else {
        for (auto& block : memory) {
            if (block.isOccupied && block.processName == processName) {
                block.isOccupied = false;
                block.processName = "";
                freeMemory += (block.endAddress - block.startAddress + 1);
                std::cout << "MemoryManager: Deallocated memory for process " << processName << " in Flat mode.\n";
            }
        }
        mergeFreeBlocks();
    }
}

/**
 * Switches to flat memory allocation mode.
 */
void MemoryManager::switchToFlat() {
    if (isPaging) {
        std::cout << "MemoryManager: Switching to Flat Memory Allocation mode.\n";
        isPaging = false;

        // Reset flat memory blocks
        memory.clear();
        memory.push_back({ 0, maxMemory - 1, false, "" }); // Reset to a single free block
        freeMemory = maxMemory; // Reset free memory
    }
}

/**
 * Switches to paging memory allocation mode.
 */
void MemoryManager::switchToPaging() {
    if (!isPaging) {
        std::cout << "MemoryManager: Switching to Paging Memory Allocation mode.\n";
        isPaging = true;

        // Reset PagingAllocator
        pagingAllocator = PagingAllocator(maxMemory);
    }
}

/**
 * Visualizes the current memory allocation.
 */
void MemoryManager::visualizeMemory() const {
    if (isPaging) {
        std::cout << "MemoryManager: Paging Mode Visualization:\n";
        pagingAllocator.visualizeMemory();
    }
    else {
        std::cout << "MemoryManager: Flat Mode Visualization:\n";
        for (const auto& block : memory) {
            std::cout << "Block [" << block.startAddress << " - " << block.endAddress << "]: ";
            std::cout << (block.isOccupied ? block.processName : "Free") << "\n";
        }
    }
}

/**
 * Calculates external fragmentation in flat mode.
 */
int MemoryManager::calculateExternalFragmentation() const {
    if (isPaging) {
        return 0; // No external fragmentation in paging mode
    }

    int fragmentation = 0;
    for (const auto& block : memory) {
        if (!block.isOccupied) {
            fragmentation += (block.endAddress - block.startAddress + 1);
        }
    }
    return fragmentation;
}

/**
 * Saves a memory snapshot to a file.
 */
void MemoryManager::saveMemorySnapshot(int quantumCycle) const {
    std::ofstream file("memory_snapshot_" + std::to_string(quantumCycle) + ".txt");
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for memory snapshot.\n";
        return;
    }

    file << "Memory Snapshot #" << quantumCycle << "\n";

    if (isPaging) {
        file << pagingAllocator.getMemoryMap();
    }
    else {
        for (const auto& block : memory) {
            file << "Block [" << block.startAddress << " - " << block.endAddress << "]: ";
            file << (block.isOccupied ? block.processName : "Free") << "\n";
        }
    }

    file.close();
}

/**
 * Merges adjacent free blocks in flat mode.
 */
void MemoryManager::mergeFreeBlocks() {
    for (auto it = memory.begin(); it != memory.end() - 1; ++it) {
        if (!it->isOccupied && !(it + 1)->isOccupied) {
            it->endAddress = (it + 1)->endAddress;
            memory.erase(it + 1);
            --it;
        }
    }
}

/**
 * Gets memory utilization as a percentage.
 */
double MemoryManager::getMemoryUtilization() const {
    return ((totalMemory - freeMemory) / totalMemory) * 100.0;
}

/**
 * Gets external fragmentation in bytes.
 */
double MemoryManager::getExternalFragmentation() const {
    return calculateExternalFragmentation();
}

/**
 * Gets the total memory size.
 */
double MemoryManager::getTotalMemory() const {
    return totalMemory;
}

/**
 * Gets the free memory size.
 */
double MemoryManager::getFreeMemory() const {
    return freeMemory;
}
