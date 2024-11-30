#pragma once
#include "IMemoryAllocator.h"
#include "Process.h"
#include <unordered_map>
#include <queue>
#include <vector>
#include <fstream>

class PagingAllocator : public IMemoryAllocator {
public:
    PagingAllocator(size_t maxMemory);
    ~PagingAllocator();

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;
    void visualizeMemory() override;
    void vmstat() const override;

private:
    size_t maxMemory;
    std::vector<size_t> freeFrameList;
    std::unordered_map<size_t, std::shared_ptr<Process>> frameMap;
    std::queue<std::shared_ptr<Process>> processQueue;
    std::ofstream backingStore;

    // VMStat related fields
    size_t pagesAllocated = 0;
    size_t pagesDeallocated = 0;

    // Helper functions
    bool allocateFrames(std::shared_ptr<Process> process, size_t numFrames);
    void deallocateFrames(std::shared_ptr<Process> process);
    void manageBackingStore(std::shared_ptr<Process> process, bool save = true);
};
