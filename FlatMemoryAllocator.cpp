#include "FlatMemoryAllocator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxMemory) :
    maximumSize(maxMemory), allocatedSize(0), memory(maxMemory, '.'), allocationMap(maxMemory, false) {
    initializeMemory();
    backingStore.open(".pagefile", std::ios::binary | std::ios::trunc);  // Open backing store file
    backingStore.close();  // Close immediately for later use
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.'); // Initialize memory with free indicator '.'
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process) {
    size_t processSize = process->getMemorySize();
    for (size_t i = 0; i <= maximumSize - processSize; ++i) {
        if (canAllocateAt(i, processSize)) {
            allocateAt(i, processSize);
            processAllocationMap[process->getProcessID()] = i; // Map process to start index
            writeBackingStore(process);  // Optionally write to backing store
            return &memory[i];
        }
    }
    std::cout << "Allocation failed for Process ID=" << process->getProcessID() << ".\n";
    return nullptr;
}

void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> process) {
    int pid = process->getProcessID();
    auto it = processAllocationMap.find(pid);
    if (it != processAllocationMap.end()) {
        size_t startIndex = it->second;
        deallocateAt(startIndex, process->getMemorySize());
        processAllocationMap.erase(it);
        readBackingStore(process);  // Optionally read from backing store
    } else {
        std::cout << "Deallocation failed for Process ID=" << pid << ": Not found.\n";
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) {
    return std::all_of(allocationMap.begin() + index, allocationMap.begin() + index + size, [](bool a){ return !a; });
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, true);
    std::fill(memory.begin() + index, memory.begin() + index + size, '#');
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index, size_t size) {
    std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, false);
    std::fill(memory.begin() + index, memory.begin() + index + size, '.');
    allocatedSize -= size;
}

void FlatMemoryAllocator::visualizeMemory() {
    std::cout << "Memory Layout:\n";
    for (char block : memory) {
        std::cout << block;
    }
    std::cout << "\nAllocated Size: " << allocatedSize << " / " << maximumSize << " KB\n";
}


void FlatMemoryAllocator::readBackingStore(std::shared_ptr<Process> process) {
    std::ifstream backingStoreFile(".pagefile");
    if (!backingStoreFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }
    
    std::unordered_map<int, std::pair<size_t, size_t>> backingStore;  // Using process ID and size pairs
    std::string line;
    int pid;
    size_t start, length;

    while (std::getline(backingStoreFile, line)) {
        std::istringstream iss(line);
        if (iss >> pid >> start >> length) {
            backingStore[pid] = std::make_pair(start, length);
        }
    }

    backingStoreFile.close();

    auto it = backingStore.find(process->getProcessID());
    if (it != backingStore.end()) {
        // Load process data into memory or handle as necessary
        std::cout << "Data for process " << process->getProcessID() << " loaded from disk.\n";
        backingStore.erase(it);
    }

    // Assuming the file needs to be updated to remove loaded data
    std::ofstream saveFile(".pagefile", std::ios::trunc);
    if (saveFile.is_open()) {
        for (const auto& pair : backingStore) {
            saveFile << pair.first << ' ' << pair.second.first << ' ' << pair.second.second << '\n';
        }
        saveFile.close();
    }
}








void FlatMemoryAllocator::writeBackingStore(std::shared_ptr<Process> process) {
    std::ifstream backingStoreFile(".pagefile");
    if (!backingStoreFile.is_open()) {
        std::cerr << "Failed to open the file for reading." << std::endl;
        return;
    }

    // Load existing data into memory
    std::unordered_map<int, std::pair<size_t, size_t>> backingStore;  // Maps process ID to memory size and command counter
    std::string line;
    int pid;
    size_t memSize, cmdCount;

    while (std::getline(backingStoreFile, line)) {
        std::istringstream iss(line);
        if (iss >> pid >> cmdCount >> memSize) {
            backingStore[pid] = std::make_pair(cmdCount, memSize);
        }
    }
    backingStoreFile.close();

    // Add or update the process's data in the backing store
    backingStore[process->getProcessID()] = std::make_pair(process->getCommandCounter(), process->getMemorySize());

    // Write the updated data back to the file
    std::ofstream saveFile(".pagefile", std::ios::trunc);
    if (!saveFile.is_open()) {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return;
    }

    for (const auto& entry : backingStore) {
        saveFile << entry.first << ' ' << entry.second.first << ' ' << entry.second.second << '\n';
    }
    saveFile.close();
}


void FlatMemoryAllocator::printProcesses() {
    std::cout << "Memory Usage: " << allocatedSize << " / " << maximumSize << " bytes\n";
    std::cout << "Memory Util: " << (static_cast<double>(allocatedSize) / maximumSize) * 100.0 << "%\n\n";

    // Header for the output
    std::cout << "Running processes and memory usage:\n";
    for (int i = 0; i < 48; i++) { std::cout << "="; }
    std::cout << "\n";

    // Display each process and its memory usage
    for (const auto& entry : processAllocationMap) {
        std::shared_ptr<Process> process = std::make_shared<Process>(); // You need to get the actual process object
        std::cout << "Process ID: " << entry.first << " Memory Used: " << entry.second << " bytes\n";
    }

    for (int i = 0; i < 48; i++) { std::cout << "-"; }
    std::cout << "\n";
}


void MemoryManager::vmstat() {
    int totalMemory = this->maxMemory * 1024; // Assuming memory is in KB
    int activeMemory = calculateActiveMemory() * 1024; // You'll need a method to calculate active memory
    int usedMemory = calculateUsedMemory() * 1024; // Similarly, calculate used memory

    int totalTicks = Scheduler::getTotalTicks();
    int activeTicks = Scheduler::getActiveTicks();
    int idleTicks = totalTicks - activeTicks;

    std::cout << "K Total Memory: " << totalMemory << " KB\n"
              << "K Used Memory: " << usedMemory << " KB\n"
              << "K Active Memory: " << activeMemory << " KB\n"
              << "Idle CPU Ticks: " << idleTicks << "\n"
              << "Active CPU Ticks: " << activeTicks << "\n"
              << "Total CPU Ticks: " << totalTicks << std::endl;
}



