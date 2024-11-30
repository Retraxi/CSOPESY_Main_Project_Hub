#pragma once
#ifndef FLATMALLOCATOR_H
#define FLATMALLOCATOR_H

#include "IMemoryAllocator.h"
#include "Process.h"

#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <unordered_map>
#include <string>

/**
 * FlatMemoryAllocator class manages memory allocation and deallocation
 * in a flat memory model. Provides functionalities for backing store handling,
 * memory visualization, and statistics.
 */
class FlatMemoryAllocator : public IMemoryAllocator {
public:
    explicit FlatMemoryAllocator(size_t maxMemory);
    ~FlatMemoryAllocator() override = default;

    /**
     * Allocates memory for the specified process.
     * @param process The process to allocate memory for.
     * @return true if allocation is successful, false otherwise.
     */
    bool allocate(std::shared_ptr<Process> process) override;

    /**
     * Deallocates the memory occupied by the specified process.
     * @param process The process whose memory is to be deallocated.
     */
    void deallocate(std::shared_ptr<Process> process) override;

    /**
     * Prints the current memory state for debugging or visualization purposes.
     */
    void printMem() override;

    /**
     * Prints a list of running processes and their memory usage.
     */
    void printProcesses() override;

    /**
     * Displays memory and CPU usage statistics.
     */
    void vmstat() override;

private:
    /**
     * Reads process information from the backing store.
     * @param process The process to load from the backing store.
     */
    void readBackingStore(std::shared_ptr<Process> process);

    /**
     * Writes process information to the backing store.
     * @param process The process to save to the backing store.
     */
    void writeBackingStore(std::shared_ptr<Process> process);

    size_t _maxMemory; // Total memory available for allocation
    size_t allocatedSize; // Currently allocated memory
    std::vector<std::pair<std::shared_ptr<Process>, std::pair<int, int>>> _memory; // Tracks memory allocations
    std::unordered_map<int, size_t> processAllocationMap; // Maps process ID to memory blocks
};

#endif // !FLATMALLOCATOR_H
