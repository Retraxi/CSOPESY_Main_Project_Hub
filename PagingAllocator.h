
#pragma once
#ifndef PAGINGALLOCATOR_H
#define PAGINGALLOCATOR_H

#include <queue>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "Process.h"

class PagingAllocator {
public:
    explicit PagingAllocator(int maxMemory);
    ~PagingAllocator() = default;

    bool allocateMemory(std::shared_ptr<Process> process);
    void freeMemory(std::shared_ptr<Process> process);

    void displayMemoryStatus() const;
    void visualizeMemory() const;
    void displayStatistics() const;

private:
    int maxMemory;
    int pagesPagedIn;
    int pagesPagedOut;

    std::queue<int> freeFrames;
    std::unordered_map<std::string, std::vector<int>> pageTable;

    void initializeFreeFrames();
    void allocateNewFrames(std::shared_ptr<Process> process, int requiredPages);
    void restoreFrames(std::shared_ptr<Process> process);
    void releaseFrames(std::shared_ptr<Process> process);
    int calculateFreeMemory() const;

    static constexpr int PAGE_SIZE = 4; 
};

#endif 
