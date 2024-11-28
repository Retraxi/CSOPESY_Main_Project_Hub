#include "MemoryManager.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <algorithm> // For std::find_if

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
            int originalEnd = block.endAddress;
            block.endAddress = allocatedEnd;
            block.isOccupied = true;
            block.processName = processName;

            // Add a new free block for any remaining memory
            if (allocatedEnd + 1 <= originalEnd) {
                auto it = std::find(memory.begin(), memory.end(), block);
                if (it != memory.end()) {
                    memory.insert(it + 1, { allocatedEnd + 1, originalEnd, false, "" });
                }
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
                break;
            }
        }
        mergeFreeBlocks();
    }
}

/**
 * Merges adjacent free blocks in flat mode.
 */
void MemoryManager::mergeFreeBlocks() {
    for (auto it = memory.begin(); it != memory.end();) {
        auto next = std::next(it);
        if (next != memory.end() && !it->isOccupied && !next->isOccupied) {
            it->endAddress = next->endAddress; // Merge blocks
            memory.erase(next);
        }
        else {
            ++it;
        }
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
 * Evicts the oldest process from flat memory allocation.
 */
void MemoryManager::evictOldestProcess() {
    auto it = std::find_if(memory.begin(), memory.end(), [](const MemoryBlock& block) {
        return block.isOccupied;
        });

    if (it != memory.end()) {
        std::cout << "Evicting process " << it->processName << " from flat memory.\n";

        // Create a Process from the MemoryBlock being evicted
        Process evictedProcess(1, it->processName, (it->endAddress - it->startAddress + 1));

        // Move to flatBackingStore (using the process name as the key)
        flatBackingStore.push_back(MemoryBlock{ it->startAddress, it->endAddress, false, it->processName });

        it->isOccupied = false;
        it->processName = "";
        freeMemory += (it->endAddress - it->startAddress + 1);

        mergeFreeBlocks();
        evictionQueue.push(evictedProcess.getName());
    }
    else {
        std::cerr << "Error: No occupied process found to evict.\n";
    }
}


/**
 * Restores a process from the backing store.
 */
bool MemoryManager::restoreProcessFromBackingStore(const std::string& processName) {
    if (!isPaging) { // If in flat memory mode
        // Iterate through the flatBackingStore to find the process
        for (auto it = flatBackingStore.begin(); it != flatBackingStore.end(); ++it) {
            if (it->processName == processName) { // Check if the process name matches
                // Create a process from the MemoryBlock (no copying)
                Process process(1, it->processName, (it->endAddress - it->startAddress + 1));
                bool allocated = allocateMemory(process.getName(), process.getMemorySize());

                if (allocated) {
                    std::cout << "Restored process " << processName << " from flat backing store to memory.\n";
                    flatBackingStore.erase(it); // Remove from flatBackingStore
                    return true;
                }
                else {
                    std::cerr << "Failed to restore process " << processName << ". Attempting eviction.\n";
                    evictOldestProcess();
                    return restoreProcessFromBackingStore(processName);
                }
            }
        }
    }
    else { // If in paging memory mode
        // Access the pageBackingStore (map) for paging memory mode
        auto it = pageBackingStore.find(processName);
        if (it != pageBackingStore.end()) {
            // Access the process by reference (no copy)
            Process& process = it->second; // Use reference here
            bool allocated = allocateMemory(process.getName(), process.getMemorySize());

            if (allocated) {
                std::cout << "Restored process " << processName << " from paging backing store to memory.\n";
                pageBackingStore.erase(it); // Remove from pageBackingStore
                return true;
            }
            else {
                std::cerr << "Failed to restore process " << processName << ". Attempting eviction.\n";
                evictOldestProcess();
                return restoreProcessFromBackingStore(processName);
            }
        }
    }

    // If the process wasn't found in either backing store
    std::cerr << "Error: Process " << processName << " not found in backing store.\n";
    return false;
}




/**
 * Evicts the oldest page from paging memory allocation.
 * Moves the evicted page to the backing store for future restoration if needed.
 */
 /**
  * Evicts the oldest page from paging memory allocation.
  * Moves the evicted page to the backing store for future restoration if needed.
  */
void MemoryManager::evictOldestPage() {
    // Check if there are any pages to evict
    if (evictionQueue.empty()) {
        std::cerr << "Error: No pages to evict in paging mode.\n";
        return;
    }

    // Get the name of the oldest process/page to evict
    std::string oldestProcessName = evictionQueue.front();
    evictionQueue.pop();

    // Check if the process exists in the pageBackingStore
    auto it = pageBackingStore.find(oldestProcessName);
    if (it == pageBackingStore.end()) {
        std::cerr << "Error: Process " << oldestProcessName << " not found in page backing store.\n";
        return;
    }

    // Retrieve the process object from the backing store by reference
    Process& process = it->second;

    // Deallocate its memory using the PagingAllocator
    pagingAllocator.deallocate(&process);

    // After eviction, the process is already in the backing store, so no need to reinsert it
    // This step might be redundant because we didn't modify the `process`
    // pageBackingStore[oldestProcessName] = process; // This line is actually unnecessary

    std::cout << "Evicted oldest page for process " << oldestProcessName
        << " and saved it to the page backing store.\n";
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
 * Gets memory utilization.
 */
double MemoryManager::getMemoryUtilization() const {
    return ((totalMemory - freeMemory) / totalMemory) * 100.0;
}

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

void MemoryManager::switchToPaging() {
    if (!isPaging) {
        std::cout << "MemoryManager: Switching to Paging Memory Allocation mode.\n";
        isPaging = true;

        // Reset PagingAllocator
        pagingAllocator = PagingAllocator(maxMemory);
    }
}

double MemoryManager::getExternalFragmentation() const {
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

double MemoryManager::getTotalMemory() const {
    return totalMemory;
}

double MemoryManager::getFreeMemory() const {
    return freeMemory;
}
