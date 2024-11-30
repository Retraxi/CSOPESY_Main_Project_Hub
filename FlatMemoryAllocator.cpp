#pragma once
#include "IMemoryAllocator.h"
#include "FlatMemoryAllocator.h"
#include <iterator>
#include "Process.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxMemory) {
	this->maximumSize = maxMemory;
	memory.reserve(maximumSize);
	initializeMemory();
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
	memory.clear();
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process) {
	size_t processMem = process->getMemorySize();

	size_t memSize = this->maximumSize - processMem;
	for (size_t i = 0; i < memSize + 1; i++)
	{
		if (!allocationMap[i] && canAllocateAt(i, process->getMemorySize())) {
			allocateAt(i, process->getMemorySize());
			return &memory[i];
		}
	}
	//nothing available
	return nullptr;
}

void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> process) {
	size_t index = reinterpret_cast<char*>(process->getMemorySize()) - &memory[0];
	if (allocationMap[index])
	{
		deallocateAt(index);
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


