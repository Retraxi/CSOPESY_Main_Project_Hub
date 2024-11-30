#pragma once
#include "IMemoryAllocator.h"
#include "FlatMemoryAllocator.h"
#include <iterator>
#include "Process.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxMemory) {
    this->maximumSize = maxMemory;
    memory.resize(maximumSize, '.'); // Initialize memory
    allocationMap.resize(maximumSize, false); // Initialize allocation map
    initializeMemory();
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
	memory.clear();
}

// Modified allocate
void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process) {
    size_t processMem = process->getMemorySize();

    if (processMem > this->maximumSize) {
        return nullptr;
    }

    for (size_t i = 0; i <= this->maximumSize - processMem; i++) {
        if (!allocationMap[i] && canAllocateAt(i, processMem)) {
            allocateAt(i, processMem);
            processMemoryMap[process] = i; // Store index
            return &memory[i];
        }
    }

    return nullptr;
}

// Modified deallocate
void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> process) {
    auto it = processMemoryMap.find(process);
    if (it != processMemoryMap.end()) { // Check if the process exists in the map
        size_t index = it->second; // Retrieve the stored index
        if (index < allocationMap.size() && allocationMap[index]) {
            deallocateAt(index);
            processMemoryMap.erase(it); // Remove the process from the map
        }
    }
}


void FlatMemoryAllocator::visualizeMemory() {
	 std::cout << std::string(memory.begin(), memory.end()) << std::endl;
}

void FlatMemoryAllocator::initializeMemory() {
	std::fill(memory.begin(), memory.end(), '.');
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) {
	return (index + size <= maximumSize);
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
	auto offset1 = std::next(allocationMap.begin(), index);
	auto offset2 = std::next(allocationMap.begin(), index + size);
	std::fill(offset1 , offset2, true);
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
	allocationMap[index] = false;
}


