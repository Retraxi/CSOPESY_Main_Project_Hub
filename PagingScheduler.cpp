#include "PagingScheduler.h"
#include <iostream>

PagingScheduler::PagingScheduler(size_t totalFrames) 
    : allocator(totalFrames) {}

void PagingScheduler::addProcess(Process* process) {
    // Attempt to allocate memory for the process
    void* memoryBlock = allocator.allocateToProcess(process->getNumPages(), *process);
    if (!memoryBlock) {
        std::cerr << "Failed to allocate memory for Process " 
                  << process->getProcessID() << ". Adding to backing store.\n";
        backingStore.push_back(process);
        return;
    }

    // If allocation is successful, add to ready queue
    readyQueue.push(process);
    std::cout << "Process " << process->getProcessID() << " added to the ready queue.\n";
}

void PagingScheduler::run() {
    while (!readyQueue.empty() || !backingStore.empty()) {
        // Execute processes in the ready queue
        while (!readyQueue.empty()) {
            Process* process = readyQueue.front();
            readyQueue.pop();

            std::cout << "Running Process " << process->getProcessID() << "...\n";
            while (!process->isDone()) {
                process->execute();
            }

            std::cout << "Process " << process->getProcessID() << " completed.\n";
            deallocateProcess(process->getProcessID());
            completedProcesses.push_back(process);
        }

        // Check backing store for processes that can now be allocated
        checkBackingStore();
    }
}

void PagingScheduler::deallocateProcess(int processID) {
    allocator.deallocate(processID);
    std::cout << "Memory deallocated for Process " << processID << ".\n";
}

void PagingScheduler::checkBackingStore() {
    auto it = backingStore.begin();
    while (it != backingStore.end()) {
        Process* process = *it;
        // Attempt to allocate memory for the process again
        void* memoryBlock = allocator.allocateToProcess(process->getNumPages(), *process);
        if (memoryBlock) {
            readyQueue.push(process);
            std::cout << "Process " << process->getProcessID() 
                      << " moved from backing store to ready queue.\n";
            it = backingStore.erase(it); // Remove from backing store
        } else {
            ++it; // Move to the next process
        }
    }
}
