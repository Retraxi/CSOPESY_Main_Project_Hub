#pragma once
#include "IMemoryAllocator.h"
#include "Process.h"
#include <unordered_map>
class PagingAllocator : public IMemoryAllocator {
public:
	PagingAllocator(size_t maxMemory);
	~PagingAllocator() = default;

	void* allocate(std::shared_ptr<Process> process) override;
	void deallocate(std::shared_ptr<Process> process) override;
	void visualizeMemory() override;

private:
	size_t maxMemory;
	size_t numFrames;
	size_t memPerFrame;
	std::unordered_map<size_t, size_t> frameMap;
	std::vector<size_t> freeFrameList;

	int pageAllocated;
	int pageDeallocated;

	//FIFO Implementation


	size_t allocateFrames(size_t numFrames, size_t processID);
	void deallocateFrames(size_t numFrames, size_t frameIndex);
};