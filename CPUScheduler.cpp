#include "CPUScheduler.h"
#include <thread>

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
	this->processList.push_back(process);
}

void CPUScheduler::initialize()
{
	//make the cpulist
	sharedInstance = new CPUScheduler(); //like in ConsoleManager
	for (size_t i = 0; i < this->coresTotal; i++)
	{
		//make a core
		sharedInstance->cpuCores.push_back(std::make_shared<Core>(i)); //0 indexed
	}
}

void CPUScheduler::destroy()
{
	delete sharedInstance;
}

void CPUScheduler::startScheduler(int choice)
{
	if (choice == 1)
	{
		if (!this->isRunning)
		{
			this->isRunning = true;
			std::thread newThread(&CPUScheduler::FCFSScheduler, this);
			newThread.detach(); //One thread for the scheduler
		}
	}
}

void CPUScheduler::FCFSScheduler()
{
	if (testing == true) {
		//create the weird processlist
		for (size_t i = 0; i < 10; i++)
		{
			//10 processes with 100 print commands
			std::shared_ptr<Process> dummy = std::make_shared<Process>(i, "process_" + i);
			dummy->testInitFCFS();
			addProcess(dummy);
		}
	}
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
				if (!this->unfinishedQueue.empty())
				{
					//get from the to be processed processes
					this->cpuCores[i]->setProcess(this->unfinishedQueue.front());
					this->unfinishedQueue.pop();
					this->isRunning = true;
				}
			}
		}
	}
}