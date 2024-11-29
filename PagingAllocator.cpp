#include "PagingAllocator.h"
#include <iostream>

PagingAllocator::PagingAllocator(size_t maxMemory) {
	this->maxMemory = maxMemory;
}

void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
	size_t processId = process->getProcessID();
	size_t numFramesNeeded = process->getMemorySize() / memPerFrame; //gets the number of frames needed
	if (numFramesNeeded <= freeFrameList.size() && process->getFrameIndices().empty()) {
		//Scenario for entirely new processes
		//std::cerr << "Memory Allocation failed, not enough free frames. " << std::endl;
		//If lacking frames -> allocate frames until full
		processOrder.push(process);
		size_t frameIndex = allocateFrames(numFramesNeeded, processId);

		return reinterpret_cast<void*>(frameIndex); //different from static cast
	}
	else {
		//check if the process is already allocated, e.g. it's been given pages and is coming back from its
		//last quantum cycle, also check if it's lacking pages compared to last time

		if (!process->getFrameIndices().empty() && numFramesNeeded == process->getFrameIndices().size())
		{
			//if allocated and not lacking then do nothing
		}

		//add in the process and take note of its age
		processOrder.push(process);

		size_t frameIndex = allocateFrames(numFramesNeeded, processId);
		return reinterpret_cast<void*>(frameIndex); //different from static cast	
	}
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