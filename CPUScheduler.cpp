#include "CPUScheduler.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

CPUScheduler* CPUScheduler::getInstance()
{
	return sharedInstance;
};

CPUScheduler::CPUScheduler()
{

}


void CPUScheduler::addProcess(std::shared_ptr<Process> process)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	//std::shared_ptr<Process> newProcess = std::make_shared<Process>(ConsoleManager::getInstance()->tableSize() + 1, command.substr(10));
	//make a screen for this process and add it to the console manager
	std::shared_ptr<Screen> newScreen = std::make_shared<Screen>(process, process->getName());
	ConsoleManager::getInstance()->registerScreen(newScreen);
	this->processList.push_back(process);
	this->unfinishedQueue.push(process);
}

void CPUScheduler::initialize(int cpuNum, std::string schedulerType, 
							  unsigned int quantumCycles, unsigned int batchProcessFreq,
							  unsigned int minIns, unsigned int maxIns,
							  unsigned int execDelay)
{
	//make the cpulist
	sharedInstance = new CPUScheduler(); //like in ConsoleManager/
	sharedInstance->coresTotal = cpuNum;
	sharedInstance->quantumCycles = quantumCycles;
	sharedInstance->batchProcessFrequency = batchProcessFreq;
	sharedInstance->minIns = minIns;
	sharedInstance->maxIns = maxIns;
	sharedInstance->execDelay = execDelay;

	for (size_t i = 1; i < sharedInstance->coresTotal + 1; i++)
	{
		//make a core
		sharedInstance->cpuCores.push_back(std::make_shared<Core>(i, execDelay)); //0 indexed
	}
	sharedInstance->startScheduler(schedulerType);
}

void CPUScheduler::destroy()
{
	delete sharedInstance;
}

void CPUScheduler::startScheduler(std::string choice)
{
	if (choice == "fcfs")
	{
		if (!this->isRunning)
		{
			this->isRunning = true;
			std::thread newThread(&CPUScheduler::FCFSScheduler, this);
			newThread.detach(); //One thread for the scheduler
		}
	}
	else if ("rr")
	{
		if (!this->isRunning)
		{
			this->isRunning = true;
			std::thread newThread(&CPUScheduler::RRScheduler, this, this->quantumCycles);
			newThread.detach(); //One thread for the scheduler
		}
	}
}

void CPUScheduler::generateProcesses() {
	std::random_device rd; 
	std::mt19937 gen(rd()); 
	std::uniform_int_distribution<> distr(this->minIns, this->maxIns);
	std::shared_ptr<Process> dummy = std::make_shared<Process>(ConsoleManager::getInstance()->tableSize() - 2, "process_" + std::to_string(ConsoleManager::getInstance()->tableSize() - 2)); // -2 pffset cause of MAIN and MARQUEE
	//set the amount of instructions based on the given range from the config
	dummy->setInstruction(distr(gen));
	addProcess(dummy);
}

void CPUScheduler::printRunningProcesses()
{
	
	for (size_t i = 0; i < cpuCores.size(); i++)
	{
		//check each core for a running process
		if (this->cpuCores[i]->getProcess() != nullptr)
		{
			//get the info needed from the process
			std::cout << std::left << std::setw(11) << this->cpuCores[i]->getProcess()->getName() << " ";
			std::cout << std::left << std::setw(23) << std::put_time(this->cpuCores[i]->getProcess()->getCreatedAt(), "(%d/%m/%Y %I:%M:%S%p) ") << "   ";
			std::cout << std::left << std::setw(7) << this->cpuCores[i]->getProcess()->getCoreID() << "   ";
			std::stringstream temp;
			temp << this->cpuCores[i]->getProcess()->getCurrentInstructionLines() << " / " << this->cpuCores[i]->getProcess()->getTotalInstructionLines();
			std::cout << std::left << std::setw(13) << temp.str() << std::endl;
		}

	}
}

void CPUScheduler::printFinishedProcesses()
{
	for (size_t i = 0; i < processList.size(); i++)
	{
		//check each core for a running process
		if (processList[i]->isDone())
		{
			//get the info needed from the process
			std::cout << std::left << std::setw(11) << processList[i]->getName() << " ";
			std::cout << std::left << std::setw(23) << std::put_time(processList[i]->getFinishedAt(), "(%d/%m/%Y %I:%M:%S%p) ") << "   ";
			std::cout << std::left << std::setw(7) << "Finished" << "   ";
			std::stringstream temp;
			temp << processList[i]->getCurrentInstructionLines() << " / " << processList[i]->getTotalInstructionLines();
			std::cout << std::left << std::setw(13) << temp.str() << std::endl;
		}
	}
}

void CPUScheduler::printCoreInfo() {
	//check the stats of the cores
	int activeCores = 0;
	int availableCores = 0;
	for (size_t i = 0; i < this->cpuCores.size(); i++) {
		if (this->cpuCores[i]->isReady()) {
			availableCores++;
		}
		else {
			activeCores++;
		}
	}

	//after checking
	int util = (activeCores / (activeCores + availableCores)) * 100;

	//printing
	std::cout << "CPU Utilization: " << util << "\%" << std::endl;
	std::cout << "Cores used: " << activeCores << std::endl;
	std::cout << "Cores available: " << availableCores << std::endl;
}

void CPUScheduler::FCFSScheduler()
{
	//std::cout << "FCFSScheduler started!" << std::endl;
	this->cycleCount = 0;
	while (this->isRunning)
	{
		for (size_t i = 0; i < cpuCores.size(); i++)
		{
			if (!this->cpuCores[i]->isRunning() && this->cpuCores[i]->isReady())
			{
				//check if the cpu core is empty
				if (this->cpuCores[i]->getProcess() != nullptr)
				{
					//check if done
					if (this->cpuCores[i]->getProcess()->isDone())
					{
						this->cpuCores[i]->setProcess(nullptr); //remove the process
					}
				}
				if(!this->unfinishedQueue.empty() && this->cpuCores[i]->getProcess() == nullptr)
				{
					//get from the to be processed processes
					this->cpuCores[i]->setProcess(this->unfinishedQueue.front());
					this->unfinishedQueue.pop();
					this->isRunning = true;
				}
			}
		}
		//end of cpu cycle
		if (cycleCount % batchProcessFrequency == 0) {
			//std::cout << "New Process generated." << std::endl;
			generateProcesses();
		}
		cycleCount++;
	}
}

void CPUScheduler::RRScheduler(int quantumCycle)
{
	//this value gets reset every now and then to get the difference
	//std::cout << "RRScheduler started!" << std::endl;
	auto begin = std::chrono::steady_clock::now();
	this->cycleCount = 0;
	//pop process out after quantum cycle passes
	while (this->isRunning)
	{
		//between the clocks steady_clock is used since it's suggested for measuring intervals
		//like quantumCycles
		auto cycleTimer = std::chrono::steady_clock::now(); //this is the active time of the too
		auto currentCycle = std::chrono::duration_cast<std::chrono::seconds>(begin - cycleTimer).count(); //kind of like a tick counter
		//Process removal section
		if (currentCycle > quantumCycle) {
			//it's `>` because it needs to remove the processes after the final tick passes
			//e.g. if quantum is 4 then this will proc at 5
			for (size_t i = 0; i < cpuCores.size(); i++)
			{
				if (this->cpuCores[i]->getProcess() != nullptr)
				{
					//get the process and put it into the queue
					this->unfinishedQueue.push(cpuCores[i]->getProcess());
					cpuCores[i]->setProcess(nullptr);
					cpuCores[i]->setReady(true);
				}
			}
			//reset the timer for next cycle
			begin = std::chrono::steady_clock::now();
		}

		//Process Assignment section
		for (size_t i = 0; i < cpuCores.size(); i++)
		{
			if (this->cpuCores[i]->getProcess() != nullptr)
			{
				//check if done
				if (this->cpuCores[i]->getProcess()->isDone())
				{
					this->cpuCores[i]->setProcess(nullptr); //remove the process
					this->cpuCores[i]->setReady(true);
				}
			}
			if (!this->unfinishedQueue.empty() && this->cpuCores[i]->getProcess() == nullptr && this->cpuCores[i]->isReady())
			{
				//get from the to be processed processes
				this->cpuCores[i]->setProcess(this->unfinishedQueue.front());
				this->unfinishedQueue.pop();
				this->isRunning = true;
			}
		}

		//end of cycle
		if (cycleCount % batchProcessFrequency == 0) {
			//std::cout << "New Process generated." << std::endl;
			generateProcesses();
		}
		//std::cout << cycleCount % batchProcessFrequency << std::endl;
		cycleCount++;

	}
}