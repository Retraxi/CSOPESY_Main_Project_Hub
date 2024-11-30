#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    explicit FlatMemoryAllocator(size_t maxMemory);
    ~FlatMemoryAllocator() override = default;

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;
    void visualizeMemory() override;

private:
    size_t maximumSize; // Total memory available for allocation
    size_t allocatedSize; // Currently allocated memory
    std::vector<char> memory; // Representation of memory: 1 char = 1 byte
    std::vector<bool> allocationMap; // Tracks which blocks are allocated
    std::unordered_map<int, size_t> processAllocationMap; // Maps process ID to starting index

    void initializeMemory(); // Initialize memory to free state
    bool canAllocateAt(size_t index, size_t size); // Check if a block can be allocated
    void allocateAt(size_t index, size_t size); // Mark memory as allocated
    void deallocateAt(size_t index, size_t size); // Mark memory as free
};
