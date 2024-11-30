#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <memory> // For std::shared_ptr
#include <iostream>
#include <cstddef> // For size_t

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maxMemory);
    ~FlatMemoryAllocator();

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;
    void visualizeMemory() override;

private:
    size_t maximumSize; // Total size of memory
    size_t allocatedSize; // Total allocated memory
    std::vector<char> memory; // Representation of memory where 1 char = 1 byte
    std::vector<bool> allocationMap; // Tracks allocated/free memory blocks

    // New map to track the memory index associated with each process
    std::unordered_map<std::shared_ptr<Process>, size_t> processMemoryMap;

    void initializeMemory(); // Initializes memory
    bool canAllocateAt(size_t index, size_t size); // Checks if allocation is possible at a given index
    void allocateAt(size_t index, size_t size); // Allocates memory at a specific index
    void deallocateAt(size_t index); // Deallocates memory at a specific index
};
