#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maxMemory);
    ~FlatMemoryAllocator() = default;

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;
    void visualizeMemory() override;

private:
    size_t maximumSize;
    size_t allocatedSize;
    std::vector<char> memory; // Representation of memory: 1 char = 1 byte
    std::vector<bool> allocationMap; // Tracks allocated memory blocks
    std::unordered_map<int, size_t> processAllocationMap; // Process ID to start index map

    void initializeMemory();
    bool canAllocateAt(size_t index, size_t size);
    void allocateAt(size_t index, size_t size);
    void deallocateAt(size_t index, size_t size);
};
