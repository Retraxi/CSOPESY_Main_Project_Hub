#pragma once
#include "Process.h"
#include <mutex>


class Core 
{
public:
	//declarations
	Core(int coreID);
	~Core() = default;
	int coreID;

	void setProcess(std::shared_ptr<Process> process); 
	std::shared_ptr<Process> getProcess();
	bool isRunning();
	
	bool isReady();
	void setReady(bool status);

	int getID();
	void shutdown();
	void run();
	void execute();

private:
	std::mutex mtx;
	bool running;
	bool ready;
	bool stop;
	int totalTicks = 0;

	int simulationDelay = 100;
	std::shared_ptr<Process> process = nullptr;
};