#pragma once
#include "Process.h"
#include <mutex>


class Core 
{
public:
	//declarations
	Core(int coreID, int execDelay);
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

	int simulationDelay;
	std::shared_ptr<Process> process = nullptr;
};



#pragma once
#ifndef CORE_H
#define CORE_H

#include <memory>
#include <string>
#include <mutex>
#include "Process.h"

class Core {
public:
    Core();
    ~Core() = default;

     void setProcess(std::shared_ptr<Process> process);
    int getId() const { return _id; };
    std::string getProcessName() const { return this->_process->getName(); };
    int getProcessCommandCounter() const { return this->_process->getCommandCounter(); };
    int getProcessCommandListSize() const { return this->_process->getCommandListSize(); };
    time_t getProcessArrivalTime() const { return this->_process->getArrivalTime(); };
    std::shared_ptr<Process> getProcess() { return this->_process; };
    int getTotalTicks() { return this->_totalTicks; };
    int getInactiveTicks() { return this->_inactiveTicks; };

    void stop() { this->_stopFlag = true; };
    bool isReady() const { return _ready; };
    void setReady() { this->_ready = true; };
    static void setMsDelay(int delay) { CPU::msDelay = delay; };

private:
     void run();
    void execute();

    std::mutex mtx;
    static int msDelay;

    static int nextID;
    int _id;
    bool _ready = true;
    bool _stopFlag = false;
    int _inactiveTicks = 0;
    int _totalTicks = 0;

    std::shared_ptr<Process> _process = nullptr;

};

#endif // !CORE_H
