#include "Process.h"
#include <unordered_map>
#include <iostream>
class IMemoryAllocator { 
public:
 virtual void* allocate(Process* process) = 0;
 virtual void deallocate(Process* process) = 0;
 virtual void visualizeMemory() const = 0; 
};


class PagingAllocator : public IMemoryAllocator { 
 public: PagingAllocator (size_t maxMemorySize);
void* allocate(Process* process) override; 
void deallocate(Process* process) override; 
void visualizeMemory() const override; 

private: size_t maxMemorySize; 
size_t numFrames; 
std::unordered_map<size_t, size_t> frameMap; 
std::vector<size_t> freeFrameList; 

size_t allocateFrames(size_t numFrames, size_t processId, const std :: vector<size_t>& pagesizes); 
void deallocateFrames(size_t numFrames, size_t frameIndex, const std:: vector<size_t>& pagesizes); 
} ;

//The paging allocator implementation
 // Implementation of PagingAllocator class 
PagingAllocator::PagingAllocator (size_t maxMemorySize)
 : maxMemorySize (maxMemorySize) , numFrames (maxMemorySize) {
 // Initialize the free frame list
 for (size_t  i = 0; i < numFrames; ++i) {
 freeFrameList.push_back (i) ;
}
}


void* PagingAllocator::allocate(Process* process) {
size_t processId = process->getId(); 
size_t numFramesNeeded = process->getNumPages() ;

if (numFramesNeeded > freeFrameList.size()) { 
std::cerr << "Memory allocation failed.  Not enough frames. \n";
return nullptr;
}

// Allocate frames for the process
 size_t frameIndex = allocateFrames(numFramesNeeded, processId);
 return reinterpret_cast<void*>(frameIndex);
}



void PagingAllocator::deallocate (Process* process) {
 size_t processId = process->getId();
// Find frames allocated to the process and deallocate
 auto it = std::find_if(frameMap.begin(), frameMap.end(),
                                            [processId] (const auto& entry) { return entry.second == processId; });



while (it != frameMap.end())
{  size_t frameIndex = it->first; 
deallocateFrames(1, frameIndex);
 it = std::find_if(frameMap.begin(), frameMap.end(),
                                          [processId] (const auto& entry) { return entry.second == processId;  });  
                                           }
}





void PagingAllocator::visualizeMemory() const {
 std::cout << "Memory Visualization:\n";

for (size_t frameIndex = 0;  frameIndex < numFrames; ++frameIndex) {
auto it = frameMap.find(frameIndex); 
           if (it != frameMap.end()) {

std::cout <<"Frame "  << frameIndex << " —>  Process " << it -> second << "\n";
} else {

     std:: cout  <<"Frame "  << frameIndex << " - >  Free \n";
}
}
std:: cout << "--------------------- \n" ;
}
  



size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processId, const std::vector<size_t>& pageSizes) {

size_t frameIndex = freeFrameList.back(); 
freeFrameList.pop_back() ;
// Map allocated frames to the process ID
 for (size_t i = 0; i < numFrames; ++i) {
 frameMap[frameIndex + i] = processId; } 
 return frameIndex; 
}



void PagingAllocator::deallocateFrames (size_t numFrames, size_t frameIndex, const std::vector<size_t>& pageSizes) {
 // Remove mapping of deallocated frames
 for (size_t i = 0; i < numFrames; ++i){
  frameMap.erase (frameIndex + i) ;
}
 // Add frames back to the free frame list
 for (size_t i = 0; i < numFrames; ++i) {
 freeFrameList.push_back(frameIndex + i); 
}

}



void PagingScheduler::addProcess (Process* process) {
 // Allocate memory for the process
 void* memoryBlock = memoryAllocator.allocate (process) ;
if (!memoryBlock) {
 std::cerr << "Failed to add process " << process->getId() << ". Memory allocation failed. \n";
return;
}
 size_t coreld = rand() % numCores; // Assign the process to a random core
 processCores[process] = coreId;
 process->setMemoryBlock(memoryBlock);
std::cout  << "Process "  <<process->getId() << " added to Core " << coreld << ".\n"; 
}


void PagingScheduler::removeProcess (Process* process) {
 // Deallocate memory for the process
 memoryAllocator.deallocate (process) ;
 processCores.erase (process) ;
std::cout  <<"Process " << process->getId() << " removed.\n";
}



void PagingScheduler::visualizeMemory() const {
  memoryAllocator.visualizeMemory();
}


void PagingScheduler::simulateExecution() {
for (auto& thread : coreThreads) {
 thread.join() ;
}
}

void PagingScheduler::coreFunction(size_t coreld) {
// <do some CPU Work>
}

