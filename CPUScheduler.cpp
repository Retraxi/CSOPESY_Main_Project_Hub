#include "CPUScheduler.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <fstream>
#include "MemoryManager.h"
CPUScheduler* CPUScheduler::sharedInstance = nullptr;
// Initialize MemoryManager with 16,384 bytes of memory and 4,096 bytes per process
MemoryManager memoryManager(16384, 4096);

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

void CPUScheduler::setTest(bool test)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	this->testing = test;
}

bool CPUScheduler::getTest()
{
	return this->testing;
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
	std::shared_ptr<Process> dummy = std::make_shared<Process>(ConsoleManager::getInstance()->tableSize() - 2,
		"process_" + std::to_string(ConsoleManager::getInstance()->tableSize() - 2)); // -2 pffset cause of MAIN and MARQUEE
	//set the amount of instructions based on the given range from the config
	dummy->setInstruction(distr(gen));
	addProcess(dummy);
}

void CPUScheduler::printRunningProcesses()
{

	for (size_t i = 0; i < cpuCores.size(); i++)
	{
		//check each core for a running process
		if (this->cpuCores[i]->getProcess() != nullptr && this->cpuCores[i]->getProcess() != NULL)// && !this->cpuCores[i]->isReady()
		{
			//get the info needed from the process
			auto tempprocess = this->cpuCores[i]->getProcess();
			if (tempprocess == nullptr || tempprocess->getCoreID() == -1) {
				std::cout << "FAILED RETREIVAL" << std::endl;
			}
			else {
				std::cout << std::left << std::setw(11) << tempprocess->getName() << " ";
				std::cout << std::left << std::setw(23) << std::put_time(tempprocess->getCreatedAt(), "(%d/%m/%Y %I:%M:%S%p) ") << "   ";
				std::cout << std::left << std::setw(7) << tempprocess->getCoreID() << "   ";
				std::stringstream temp;
				temp << tempprocess->getCurrentInstructionLines() << " / " << tempprocess->getTotalInstructionLines();
				std::cout << std::left << std::setw(13) << temp.str() << std::endl;
			}
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
	double activeCores = 0;
	double availableCores = 0;
	for (size_t i = 0; i < this->cpuCores.size(); i++) {
		if (this->cpuCores[i]->isReady()) {
			availableCores++;
		}
		else {
			activeCores++;
		}
	}

	//after checking
	double util = (activeCores / (activeCores + availableCores));
	util *= 100;

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
		}
		//end of cpu cycle
		if (cycleCount % batchProcessFrequency == 0 && this->testing == true) {
			//std::cout << "New Process generated." << std::endl;
			generateProcesses();
			cycleCount++;
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
		if (quantumCycle > currentCycle) {
			//std::cout << "Clearing cores..." << std::endl;
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//it's `>` because it needs to remove the processes after the final tick passes
			//e.g. if quantum is 4 then this will proc at 5
			for (size_t i = 0; i < cpuCores.size(); i++)
			{
				if (this->cpuCores[i]->getProcess() != nullptr && !this->cpuCores[i]->getProcess()->isDone() && !this->cpuCores[i]->isReady())
				{
					//get the process and put it into the queue
					//std::cout << "Removing process: " << this->cpuCores[i]->getProcess()->getName() << " at Core: " << i <<  std::endl;
					this->unfinishedQueue.push(cpuCores[i]->getProcess());
					auto process = this->cpuCores[i]->getProcess();
					this->cpuCores[i]->setProcess(nullptr);
					process->setCoreID(-1);
					this->cpuCores[i]->setReady(true);
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				else if (this->cpuCores[i]->getProcess() != nullptr && this->cpuCores[i]->getProcess()->isDone()) {
					// do not add to unfinishedQueue
					auto process = this->cpuCores[i]->getProcess();
					this->cpuCores[i]->setProcess(nullptr);
					process->setCoreID(-1);
					this->cpuCores[i]->setReady(true);
				}
			}
			//reset the timer for next cycle
			begin = std::chrono::steady_clock::now();
		}

		//Process Assignment section
		for (size_t i = 0; i < cpuCores.size(); i++)
		{
			//print the status of the Core abd the Queue
			if (this->cpuCores[i]->getProcess() != nullptr)
			{
				//check if done
				//std::cout << "Core " << i << " contains: " << this->cpuCores[i]->getProcess()->getName() << std::endl;
				//std::cout << "Ready status: " << cpuCores[i]->isReady() << std::endl;
				if (this->cpuCores[i]->getProcess()->isDone())
				{
					//remove from queue permanently
					this->cpuCores[i]->setProcess(nullptr); //remove the process
					this->cpuCores[i]->setReady(true);
				}
			}
			if (!this->unfinishedQueue.empty() && this->cpuCores[i]->getProcess() == nullptr && this->cpuCores[i]->isReady())
			{
				auto nextProcess = this->unfinishedQueue.front();
				this->unfinishedQueue.pop();

				// Attempt to allocate memory for the process
				// New code added to handle memory allocation
				if (!memoryManager.allocateMemory(nextProcess->getName())) {
					// If allocation fails, requeue the process
					this->unfinishedQueue.push(nextProcess);
					continue; // Skip this cycle if allocation fails
				}

				// Assign the process to the core if memory allocation is successful
				this->cpuCores[i]->setProcess(nextProcess);
				this->cpuCores[i]->setReady(false);
				this->cpuCores[i]->getProcess()->setCoreID(i);
				this->isRunning = true;
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

		// Deallocate memory upon process completion
		// New code added to handle memory deallocation
		for (size_t i = 0; i < cpuCores.size(); i++) {
			if (cpuCores[i]->getProcess() != nullptr && cpuCores[i]->getProcess()->isDone()) {
				// Free memory for completed process
				memoryManager.deallocateMemory(cpuCores[i]->getProcess()->getName());
				cpuCores[i]->setProcess(nullptr);
				cpuCores[i]->setReady(true);
			}
		}

		//end of cycle
		if (cycleCount % batchProcessFrequency == 0 && this->testing == true) {
			//std::cout << "New Process generated." << std::endl;
			generateProcesses();
		}
		//std::cout << cycleCount % batchProcessFrequency << std::endl;
		cycleCount++;

		// Save memory snapshot every 4 quantum-cycles
		// New code added to handle periodic memory snapshot
		if (cycleCount % 4 == 0) {
			memoryManager.saveMemorySnapshot(quantumCycle);
		}
	}
}





void CPUScheduler::generateReport() {
	std::ofstream reportFile("csopesy-log.txt");
	if (!reportFile.is_open()) {
		std::cerr << "Error: Unable to create report file." << std::endl;
		return;
	}

	reportFile << "CPU Utilization Report\n";
	reportFile << "--------------------------------------------------\n";

	// Print core info
	double activeCores = 0;
	double availableCores = 0;
	for (const auto& core : cpuCores) {
		if (core->isReady()) {
			availableCores++;
		}
		else {
			activeCores++;
		}
	}
	double utilization = (activeCores / (activeCores + availableCores)) * 100;
	reportFile << "CPU Utilization: " << utilization << "%\n";
	reportFile << "Cores used: " << activeCores << "\n";
	reportFile << "Cores available: " << availableCores << "\n";
	reportFile << "--------------------------------------------------\n";

	// Print running processes
	reportFile << "Running processes:\n";
	for (const auto& core : cpuCores) {
		auto process = core->getProcess();
		if (process != nullptr) {
			reportFile << "Process: " << process->getName()
				<< ", Core ID: " << process->getCoreID()
				<< ", Instruction Line: " << process->getCurrentInstructionLines()
				<< " / " << process->getTotalInstructionLines() << "\n";
		}
	}

	// Print finished processes
	reportFile << "\nFinished processes:\n";
	for (const auto& process : processList) {
		if (process->isDone()) {
			reportFile << "Process: " << process->getName()
				<< ", Finished at: " << std::put_time(process->getFinishedAt(), "(%d/%m/%Y %I:%M:%S%p)")
				<< ", Instruction Line: " << process->getTotalInstructionLines() << " / " << process->getTotalInstructionLines() << "\n";
		}
	}

	reportFile << "--------------------------------------------------\n";

	reportFile.close();
	std::cout << "CPU utilization report saved to csopesy-log.txt." << std::endl;
}


void CPUScheduler::reportUtilization() {
    printCoreInfo();

    // Report Memory Utilization and Fragmentation
    double memoryUtilization = memoryManager.getMemoryUtilization();
    double externalFragmentation = memoryManager.getExternalFragmentation();

    std::cout << "Memory Utilization: " << memoryUtilization << "%" << std::endl;
    std::cout << "External Fragmentation: " << externalFragmentation << " bytes" << std::endl;

  
}
