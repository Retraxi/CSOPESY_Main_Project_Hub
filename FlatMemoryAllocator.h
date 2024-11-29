#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
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
	std::vector<char> memory; //representation of memory wher 1 char = 1 byte
	std::unordered_map<size_t, bool> allocationMap;

	void initializeMemory();
	bool canAllocateAt(size_t index, size_t size);
	void allocateAt(size_t index, size_t size);
	void deallocateAt(size_t index);

};