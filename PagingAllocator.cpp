
#include "PagingAllocator.h"
#include "Process.h"
#include "Scheduler.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <ctime>

class PagingAllocator {
public:
    explicit PagingAllocator(int maxMemory) : maxMemory(maxMemory), pagesPagedIn(0), pagesPagedOut(0) {
        std::ofstream(".pagefile", std::ios::trunc).close();
        initializeFreeFrames();
    }

    bool allocateMemory(std::shared_ptr<Process> process) {
        int requiredPages = process->getRequiredPages();
        if (pageTable.count(process->getName())) {
            restoreFrames(process);
            return true;
        }
        else if (freeFrames.size() >= requiredPages) {
            allocateNewFrames(process, requiredPages);
            return true;
        }
        return false;
    }

    void freeMemory(std::shared_ptr<Process> process) {
        if (pageTable.count(process->getName())) {
            releaseFrames(process);
            pageTable.erase(process->getName());
        }
    }

    void displayMemoryStatus() const {
        std::cout << "Memory Status:\n"
            << "Processes in Memory: " << pageTable.size() << "\n"
            << "Free Memory: " << calculateFreeMemory() << " KB\n"
            << "Pages Paged In: " << pagesPagedIn << "\n"
            << "Pages Paged Out: " << pagesPagedOut << "\n";
    }

    void visualizeMemory() const {
        for (const auto& [process, frames] : pageTable) {
            std::cout << "Process: " << process << " - Frames: ";
            for (int frame : frames) {
                std::cout << frame << " ";
            }
            std::cout << "\n";
        }
    }

    void displayStatistics() const {
        std::cout << "Paging Statistics:\n"
            << "Paged In: " << pagesPagedIn << "\n"
            << "Paged Out: " << pagesPagedOut << "\n";
    }


private:
    int maxMemory;
    int pagesPagedIn;
    int pagesPagedOut;
    std::queue<int> freeFrames;
    std::unordered_map<std::string, std::vector<int>> pageTable;

    void initializeFreeFrames() {
        int totalFrames = maxMemory / PAGE_SIZE;
        for (int i = 0; i < totalFrames; ++i) {
            freeFrames.push(i);
        }
    }

    void allocateNewFrames(std::shared_ptr<Process> process, int requiredPages) {
        std::vector<int> allocatedFrames;
        for (int i = 0; i < requiredPages; ++i) {
            int frame = freeFrames.front();
            freeFrames.pop();
            allocatedFrames.push_back(frame);
            ++pagesPagedIn;
        }
        pageTable[process->getName()] = allocatedFrames;
    }

    void restoreFrames(std::shared_ptr<Process> process) {
        auto& frames = pageTable[process->getName()];
        for (int& frame : frames) {
            if (frame < 0) {
                frame = freeFrames.front();
                freeFrames.pop();
                ++pagesPagedIn;
            }
        }
    }

    void releaseFrames(std::shared_ptr<Process> process) {
        auto& frames = pageTable[process->getName()];
        for (int frame : frames) {
            freeFrames.push(frame);
            ++pagesPagedOut;
        }
    }

    int calculateFreeMemory() const {
        return freeFrames.size() * PAGE_SIZE;
    }

    static constexpr int PAGE_SIZE = 4; // 4 KB per frame
};
