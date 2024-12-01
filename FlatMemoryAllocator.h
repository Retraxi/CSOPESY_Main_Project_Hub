#pragma once
#ifndef FLATMALLOCATOR_H
#define FLATMALLOCATOR_H

#include "IMemoryAllocator.h"
#include "Process.h"

#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <unordered_map>
#include <string>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    explicit FlatMemoryAllocator(size_t maxMemory);
    ~FlatMemoryAllocator() override = default;

   
    bool allocate(std::shared_ptr<Process> process) override;

   
    void deallocate(std::shared_ptr<Process> process) override;


    void printMem() override;


    void printProcesses() override;


    void vmstat() override;

private:
    
    void readBackingStore(std::shared_ptr<Process> process);

 
    void writeBackingStore(std::shared_ptr<Process> process);

    size_t _maxMemory; 
    size_t allocatedSize; 
    std::vector<std::pair<std::shared_ptr<Process>, std::pair<int, int>>> _memory; 
    std::unordered_map<int, size_t> processAllocationMap;
};

#endif // !FLATMALLOCATOR_H
