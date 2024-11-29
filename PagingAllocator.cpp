#include "PagingAllocator.h"
#include <iostream>

PagingAllocator::PagingAllocator(size_t maxMemory) {
	this->maxMemory = maxMemory;
}

void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
	size_t processId = process->getProcessID();
	size_t numFramesNeeded = process->getNumPages();
	if (numFramesNeeded > freeFrameList.size()) {
		std::cerr << "Memory Allocation failed, not enough free frames. " << std::endl;
		return nullptr;
	}

	size_t frameIndex = allocateFrames(numFramesNeeded, processId);
	return reinterpret_cast<void*>(frameIndex); //different from static cast	
}

void PagingAllocator::deallocate(std::shared_ptr<Process> process) {
	size_t processId = process->getProcessID();	

	//iterate through frameMap and find the places where the 2nd entry of the pair is equivalent to the processID
	auto it = std::find_if(frameMap.begin(), frameMap.end(), 
		[processId](const auto& entry) {return entry.second == processId; });

	while (it != frameMap.end()) {
		size_t frameIndex = it->first;
		deallocateFrames(1, frameIndex); 
		//find the next
		it = std::find_if(frameMap.begin(), frameMap.end(),
			[processId](const auto& entry) {return entry.second == processId; });
	}
}

size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processID) {
	size_t frameIndex = freeFrameList.back();
	freeFrameList.pop_back();

	for (size_t i = 0; i < numFrames; i++)
	{
		frameMap[frameIndex + i] = processID;
	}

	return frameIndex;
}

void PagingAllocator::deallocateFrames (size_t numFrames, size_t frameIndex) {
	for (size_t i = 0; i < numFrames; i++)
	{
		frameMap.erase(frameIndex + i);
	}
	
	for (size_t i = 0; i < numFrames; i++)
	{
		freeFrameList.push_back(frameIndex + i);
	}
}

void PagingAllocator::visualizeMemory() {
	for (size_t frameIndex = 0; frameIndex < numFrames; frameIndex++)
	{
		auto it = frameMap.find(frameIndex);

		if (it != frameMap.end())
		{
			std::cout << "Frame " << frameIndex << " -> Process" << it->second << std::endl;
		}
		else {
			std::cout << "Frame " << frameIndex << " Free" << std::endl;
		}
	}
}