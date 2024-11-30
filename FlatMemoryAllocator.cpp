#include "FlatMemoryAllocator.h"
#include "Process.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxMemory)
    : maximumSize(maxMemory), allocatedSize(0), memory(maxMemory, '.'), allocationMap(maxMemory, false) {
    initializeMemory();
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.'); // Initialize memory with '.'
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process) {
    size_t processSize = process->getMemorySize();
    for (size_t i = 0; i <= maximumSize - processSize; ++i) {
        if (canAllocateAt(i, processSize)) {
            allocateAt(i, processSize);
            processAllocationMap[process->getProcessID()] = i; // Store the start index
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
    } else {
        std::cout << "Deallocation failed for Process ID=" << pid << ": Not found.\n";
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap[i]) return false;
    }
    return true;
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = true;
        memory[i] = '#'; // Mark allocated memory
    }
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = false;
        memory[i] = '.'; // Reset memory to free state
    }
    allocatedSize -= size;
}

void FlatMemoryAllocator::visualizeMemory() {
    for (char block : memory) {
        std::cout << block;
    }
    std::cout << "\n";
}
