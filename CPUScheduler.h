#pragma once
#include "Core.h"
#include <queue>

class CPUScheduler
{
	//this will be the detached thread
public:
	 void initialize();
	static CPUScheduler* getInstance();
	static void destroy();

	void addProcess(std::shared_ptr<Process> process);

	void startScheduler(int choice);
	//void run();
	//void execute();

private:

	CPUScheduler();
	~CPUScheduler() = default;
	static CPUScheduler* sharedInstance;

	void FCFSScheduler();

	bool testing = true;
	bool isRunning = false;
	int coresTotal = 4;

	std::queue<std::shared_ptr<Process>> unfinishedQueue; //non-permanent queue for unfinished processes
	std::vector<std::shared_ptr<Core>> cpuCores; //permanent cores
	std::vector<std::shared_ptr<Process>> processList; //permanent process list
};