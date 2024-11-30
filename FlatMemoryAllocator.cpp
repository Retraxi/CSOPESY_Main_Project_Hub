#include "FlatMemoryAllocator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxMemory) :
    maximumSize(maxMemory), allocatedSize(0), memory(maxMemory, '.'), allocationMap(maxMemory, false) {
    initializeMemory();
    backingStore.open(".pagefile", std::ios::binary | std::ios::trunc);  // Open backing store file
    backingStore.close();  // Close immediately for later use
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.'); // Initialize memory with free indicator '.'
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process) {
    size_t processSize = process->getMemorySize();
    for (size_t i = 0; i <= maximumSize - processSize; ++i) {
        if (canAllocateAt(i, processSize)) {
            allocateAt(i, processSize);
            processAllocationMap[process->getProcessID()] = i; // Map process to start index
            writeBackingStore(process);  // Optionally write to backing store
            return &memory[i];
        }
    }
    std::cout << "Allocation failed for Process ID=" << process->getProcessID() << ".\n";
    return nullptr;
}

void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> process) {
    int pid = process->getProcessID();
    auto it = processAllocationMap.find(pid);
    if (it != processAllocationMap.end()) {
        size_t startIndex = it->second;
        deallocateAt(startIndex, process->getMemorySize());
        processAllocationMap.erase(it);
        readBackingStore(process);  // Optionally read from backing store
    } else {
        std::cout << "Deallocation failed for Process ID=" << pid << ": Not found.\n";
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) {
    return std::all_of(allocationMap.begin() + index, allocationMap.begin() + index + size, [](bool a){ return !a; });
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, true);
    std::fill(memory.begin() + index, memory.begin() + index + size, '#');
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index, size_t size) {
    std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, false);
    std::fill(memory.begin() + index, memory.begin() + index + size, '.');
    allocatedSize -= size;
}

void FlatMemoryAllocator::visualizeMemory() {
    std::cout << "Memory Layout:\n";
    for (char block : memory) {
        std::cout << block;
    }
    std::cout << "\nAllocated Size: " << allocatedSize << " / " << maximumSize << " KB\n";
}

void FlatMemoryAllocator::vmstat() const {
    std::cout << "VM Statistics:\n"
              << "Total Memory: " << maximumSize << " bytes\n"
              << "Allocated Memory: " << allocatedSize << " bytes\n";
}

void FlatMemoryAllocator::readBackingStore(std::shared_ptr<Process> process) {
    // Logic to read from backing store
}

void FlatMemoryAllocator::writeBackingStore(std::shared_ptr<Process> process) {
    // Logic to write to backing store
}
