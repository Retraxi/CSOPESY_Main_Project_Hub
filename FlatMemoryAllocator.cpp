#include "FlatMemoryAllocator.h"
#include "Process.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxMemory)
    : maximumSize(maxMemory), allocatedSize(0), memory(maxMemory, '.'), allocationMap(maxMemory, false) {
    initializeMemory();
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.'); // Initialize memory with free indicator '.'
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process) {
    size_t processSize = process->getMemorySize();
    
    // Check for available space
    for (size_t i = 0; i <= maximumSize - processSize; ++i) {
        if (canAllocateAt(i, processSize)) {
            allocateAt(i, processSize);
            processAllocationMap[process->getProcessID()] = i; // Map process to start index
            return &memory[i];
        }
    }
    
    // Failed allocation
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
        if (allocationMap[i]) return false; // Check if block is already allocated
    }
    return true;
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = true; // Mark block as allocated
        memory[i] = '#'; // Visual indicator for allocated block
    }
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = false; // Mark block as free
        memory[i] = '.'; // Reset visual indicator
    }
    allocatedSize -= size;
}

void FlatMemoryAllocator::visualizeMemory() {
    for (char block : memory) {
        std::cout << block;
    }
    std::cout << "\n";
    std::cout << "Allocated Size: " << allocatedSize << " / " << maximumSize << " KB\n";
}
