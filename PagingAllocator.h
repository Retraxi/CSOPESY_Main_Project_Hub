#pragma once
#include "IMemoryAllocator.h"
#include "Process.h"
#include <unordered_map>
#include <queue>
class PagingAllocator : public IMemoryAllocator {
public:
	PagingAllocator(size_t maxMemory);
	~PagingAllocator();

	void* allocate(std::shared_ptr<Process> process) override;
	void deallocate(std::shared_ptr<Process> process) override;
	void visualizeMemory() override;

private:
	size_t maxMemory;
	size_t numFrames;
	size_t memPerFrame;
	size_t allocatedSize; //vmstat
	std::unordered_map<size_t, size_t> frameMap; 
	std::vector<size_t> freeFrameList; //list of free frames
	std::vector<std::shared_ptr<Process>> processOrder;

	int pagesAllocated = 0;
	int pagesDeallocated = 0;

	//FIFO Implementation
	//void evictOldestPage9();

	size_t allocateFrames(size_t numFrames, size_t processID);
	void deallocateFrames(size_t numFrames, size_t frameIndex);
};