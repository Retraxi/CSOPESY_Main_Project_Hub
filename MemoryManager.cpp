#include "MemoryManager.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <iostream>

/**
 * Initializes the MemoryManager with the maximum memory and process memory size.
 * Sets up the initial free memory block for allocation.
 */
MemoryManager::MemoryManager(int maxMemory, int processMemory)
    : maxMemory(maxMemory), processMemory(processMemory) {
    memory.push_back({ 0, maxMemory - 1, false, "" }); // Initial free block
}

/**
 * Allocates memory for a process using the first-fit allocation strategy.
 * - Finds the first available block with sufficient size.
 * - Splits the block if extra space remains after allocation.
 * @param processName Name of the process requesting memory.
 * @return True if memory allocation succeeded, otherwise False.
 */
bool MemoryManager::allocateMemory(const std::string& processName) {
    for (auto block : memory) {  // Use `auto` instead of `auto&` to avoid referencing the original blocks
        if (!block.isOccupied && (block.endAddress - block.startAddress + 1) >= processMemory) {
            int end = block.startAddress + processMemory - 1; // Calculate end of allocated range
            
            // Set the allocated block properties
            block.endAddress = end;
            block.isOccupied = true;
            block.processName = processName;

            memory.push_back(block); // Push the updated block

            // Create and push the remaining free block, if any
            if (end + 1 <= block.endAddress) {
                memory.push_back({ end + 1, block.endAddress, false, "" });
            }
            
            return true;
        }
    }
    return false; // No available block found
}

/**
 * Frees the memory occupied by a specific process and merges adjacent free blocks.
 * @param processName Name of the process to free memory for.
 */
void MemoryManager::deallocateMemory(const std::string& processName) {
    for (auto& block : memory) {
        if (block.isOccupied && block.processName == processName) {
            block.isOccupied = false;
            block.processName = "";
        }
    }
    mergeFreeBlocks(); // Consolidate adjacent free blocks
}

/**
 * Calculates external fragmentation, the total memory of unused blocks.
 * @return Total external fragmentation in bytes.
 */
int MemoryManager::calculateExternalFragmentation() const {
    int fragmentation = 0;
    for (const auto& block : memory) {
        if (!block.isOccupied) {
            fragmentation += (block.endAddress - block.startAddress + 1);
        }
    }
    return fragmentation;
}

/**
 * Saves a snapshot of the current memory allocation state to a file.
 * Includes timestamp, number of processes in memory, external fragmentation,
 * and a detailed ASCII representation of memory blocks.
 * @param quantumCycle Current quantum cycle number.
 */
void MemoryManager::saveMemorySnapshot(int quantumCycle) const {
    std::ofstream file("memory_stamp_" + std::to_string(quantumCycle) + ".txt");
    if (!file.is_open()) {
        std::cerr << "Error creating memory snapshot file." << std::endl;
        return;
    }

    // Timestamp
    auto t = std::time(nullptr);
    struct tm timeInfo;
    localtime_s(&timeInfo, &t);
    file << "Timestamp: (" << std::put_time(&timeInfo, "%d/%m/%Y %I:%M:%S%p") << ")\n";

    // Process count
    int processCount = 0;
    for (const auto& block : memory) {
        if (block.isOccupied) processCount++;
    }
    file << "Number of processes in memory: " << processCount << "\n";
    file << "Total external fragmentation in KB: " << calculateExternalFragmentation() / 1024 << "\n";

    // Memory map
    file << "----end---- = " << maxMemory << "\n";
    for (auto it = memory.rbegin(); it != memory.rend(); ++it) {
        file << it->endAddress + 1 << "\n" << (it->isOccupied ? it->processName : "Unused") << "\n" << it->startAddress << "\n";
    }
    file << "----start---- = 0\n";
    file.close();
}

/**
 * Merges adjacent free blocks to reduce memory fragmentation.
 * Called automatically after memory deallocation.
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


double MemoryManager::getMemoryUtilization() const {
    // Calculate memory utilization as a percentage of memory in use
    return ((totalMemory - freeMemory) / totalMemory) * 100;
}


double MemoryManager::getExternalFragmentation() const {
    // Calculate external fragmentation: sum of free blocks that cannot be used
    double fragmentedMemory = 0.0;
    for (double block : freeBlocks) {
        if (block < minBlockSizeForAllocation) {  // Define a minimum block size that is useful
            fragmentedMemory += block;
        }
    }
    return fragmentedMemory;
}

double MemoryManager::getTotalMemory() const {
    return totalMemory;
}

double MemoryManager::getFreeMemory() const {
    return freeMemory;
}