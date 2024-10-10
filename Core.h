#pragma once
#include "Process.h"


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

	int getID();
	void run();
	void execute();

private:
	bool running;
	bool ready;

	int simulationDelay = 50;
	std::shared_ptr<Process> process = nullptr;
};