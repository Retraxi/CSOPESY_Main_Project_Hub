#include "Core.h"
#include <thread>

Core::Core(int coreID, int execDelay)
{
	this->stop = false;
	this->coreID = coreID;
	this->ready = true;
	this->running = false;
	this->simulationDelay = execDelay;
	std::thread runningThread(&Core::run, this); //One thread per Core worker
	runningThread.detach();
}	

void Core::shutdown()
{
	this->stop = true;
}

int Core::getID()
{
	return this->coreID;
}

void Core::run()
{
	while (!this->stop) {
		
		this->execute();
		std::this_thread::sleep_for(std::chrono::milliseconds(simulationDelay)); //allows for some time difference
		this->totalTicks++;
	}
	this->running = true;
}

void Core::execute()
{
	//this will be the function to be continually called
	std::lock_guard<std::mutex> lock(this->mtx); //files were going haywire without this
	if (this->process != nullptr && !this->process->isDone())
	{
		this->process->setCoreID(this->coreID);
		this->process->execute();
		if (this->process->isDone())
		{
			//deallocate the process
			this->ready = true;
			this->running = false;
		}
	}
}

bool Core::isReady()
{
	return this->ready;
}

void Core::setReady(bool status)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	this->ready = status;
}

void Core::setProcess(std::shared_ptr<Process> process)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	this->process = process;
	if (this->process == nullptr)
	{
		this->ready = true;
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

