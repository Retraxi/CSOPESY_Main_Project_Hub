#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize) {
	memory.reserve(maximumSize); //create the "memory heap"
	this->initializeMemory();
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
	memory.clear();
}

void FlatMemoryAllocator::initializeMemory() {
	std::fill(memory.begin(), memory.end(), '.');
	std::fill(allocationMap.begin(), allocationMap.end(), false);
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
	return (index + size <= maximumSize);
}
 
void FlatMemoryAllocator::allocateAt(size_t index, size_t size){
	auto start = allocationMap.begin() + index;
	std::fill(, allocationMap.begin() + index + size, true);
}

void* FlatMemoryAllocator::allocate(size_t size) {
	for (size_t i = 0; i < maximumSize - size; i++) {
		if (!allocationMap[i] && canAllocateAt(i, size)) {
			allocateAt(i, size);
			return &memory[i];
		}
	}
}

void FlatMemoryAllocator::deallocate(void* ptr) {
	size_t index = static_cast<char*>(ptr) - &memory[0];
	if (allocationMap[index]) {
		deallocateAt(index);
	}
}

std::string FlatMemoryAllocator::visualizeMemory() {
	return std::string(memory.begin(), memory.end());
}
