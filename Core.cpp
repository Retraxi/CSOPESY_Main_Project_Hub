#include "Core.h"
#include <thread>

Core::Core(int coreID)
{
	this->coreID = coreID;
	this->ready = true;
	this->running = false;
	std::thread runningThread(&Core::run, this); //One thread per Core worker
}	

int Core::getID()
{
	return this->coreID;
}

void Core::run()
{
	while (true) {
		this->execute();
		std::this_thread::sleep_for(std::chrono::milliseconds(simulationDelay)); //allows for some time difference
	}

}

void Core::execute()
{
	//this will be the function to be continually called
	if (this->process != nullptr && !this->process->isDone())
	{
		this->process->setCoreID(this->coreID);
		this->process->execute();
		if (this->process->isDone())
		{
			//deallocate the process
			this->ready = true;
			this->process = nullptr;
			this->running = false;
		}
	}
}

bool Core::isReady()
{
	return this->ready;
}

void Core::setProcess(std::shared_ptr<Process> process)
{
	if (this->process == nullptr)
	{
		//ready
		this->ready = true;
		this->process = process;
	}
	else
	{
		//there's a process already
		//maybe an overwrite function in the future
	}
}

std::shared_ptr<Process> Core::getProcess()
{
	return this->process;
}

bool Core::isRunning()
{
	return this->running;
}

