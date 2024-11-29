#pragma once
#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <ctime> // For std::tm

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096 // 4KB page size for paging systems
#endif

class Process {
    /* Class Overview:
       - Core ID: Identifies the CPU core running the process.
       - Instruction List: List of commands/instructions the process will execute.
       - Process State: Tracks execution progress and completion status.
       - Time Stamps: Created and finished timestamps.
       - Memory: Memory requirements for paging systems.
    */

public:
    // Constructor for processes without memory requirements
    Process(int pid, std::string name);

    // Constructor for processes with memory requirements
    Process(int pid, std::string name, size_t memorySize);

    // Default destructor
    ~Process() = default;

    // Getters for process attributes
    std::string getName() const;
    int getCommandCounter() const;
    int getProcessID() const;
    int getCoreID() const;
    int getTotalInstructionLines() const;
    int getCurrentInstructionLines() const;

    // Memory-related getters
    size_t getMemorySize() const; // Returns the memory size of the process
    size_t getNumPages() const;   // Calculates the number of pages needed for the process

    // Time-related methods
    tm* getCreatedAt();           // Returns the creation timestamp
    tm* getFinishedAt();          // Returns the finished timestamp
    void setFinishedAt(tm* finishedAt); // Sets the finished timestamp

    // Setters and modifiers
    void setCoreID(int coreID);   // Assigns a CPU core to the process
    void setInstruction(int totalCount); // Adds a specified number of instructions
    void testInitFCFS();          // Initializes test instructions for First-Come-First-Serve scheduling

    // Execution-related methods
    void execute();               // Executes the current instruction
    bool isDone();                // Checks if the process has completed all instructions

private:
    std::mutex mtx;               // Mutex for thread-safe updates

    // Process attributes
    int pid;                      // Process ID
    int coreID;                   // Assigned CPU core ID
    int currentInstructionLine;   // Current instruction being executed
    size_t memorySize;            // Memory size required for the process (used in paging)
    std::tm* createdAt;           // Timestamp when the process was created
    std::tm* finishedAt;          // Timestamp when the process was finished
    std::string processName;      // Name of the process
    std::vector<std::string> instructionList; // List of instructions/commands
    int commandCounter;  //added command counter
};
