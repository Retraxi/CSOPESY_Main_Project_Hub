#pragma once
#ifndef FLATALLOCATOR_H
#define FLATALLOCATOR_H

#include "IMemoryAllocator.h"
#include "Process.h"

#include <memory>
#include <vector>
#include <utility>
#include <string>


class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(int maxMemory);
    ~FlatMemoryAllocator() = default;

    bool allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;

    void printMem() override;

    void printProcesses() override;
    void vmstat() override;

private:
    void readBackingStore(std::shared_ptr<Process> process);
    void writeBackingStore(std::shared_ptr<Process> process);
    int _maxMemory;
    std::vector<std::pair<std::shared_ptr<Process>, std::pair<int, int>>> _memory;
};

#endif //!FLATALLOCATOR_H