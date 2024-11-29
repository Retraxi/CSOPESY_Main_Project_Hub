#pragma once
#include "Core.h"
#include "ConsoleManager.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"
#include <queue>

class CPUScheduler
{
	//this will be the detached thread
public:
	static void initialize(int cpuNum, std::string schedulerType,
		unsigned int quantumCycles, unsigned int batchProcessFreq,
		unsigned int minIns, unsigned int maxIns,
		unsigned int execDelay, int maxMemory,
		int minMPP, int maxMPP);
	static CPUScheduler* getInstance();
	static void destroy();

	void addProcess(std::shared_ptr<Process> process);
	bool getTest();
	void setTest(bool test);

	void startScheduler(std::string choice);
	void printRunningProcesses();
	void printFinishedProcesses();
	void printCoreInfo();
	void generateProcesses();
	void generateReport();
    void reportUtilization();
    // void run();
    // void execute();

private:
	
	CPUScheduler();
	~CPUScheduler() = default;
	static CPUScheduler* sharedInstance;

	void FCFSScheduler();
	void RRScheduler(int quantumCycle);
	
	std::mutex mtx;

	bool testing = false;
	bool isRunning = false;
	int coresTotal;
	int cycleCount = 0;

	unsigned int  quantumCycles;
	unsigned int batchProcessFrequency;
	unsigned int minIns;
	unsigned int maxIns;
	unsigned int execDelay;

	std::queue<std::shared_ptr<Process>> unfinishedQueue; //non-permanent queue for unfinished processes
	std::vector<std::shared_ptr<Core>> cpuCores; //permanent cores
	std::vector<std::shared_ptr<Process>> processList; //permanent process list

	FlatMemoryAllocator* flatmem;
	PagingAllocator* pagingAlloc;
	int minMPP;
	int maxMPP;

	friend class ConsoleManager;
};