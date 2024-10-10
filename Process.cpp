#define _CRT_SECURE_NO_WARNINGS
#include "Process.h"
#include <fstream>
#include <iostream>
#include <iomanip>

Process::Process(int pid, std::string name) : pid(pid), processName(name)
{
	this->coreID = 0;
	std::time_t now = std::time(nullptr);
	createdAt = std::localtime(&now); //need this for scheduling
	finishedAt = nullptr;
	this->currentInstructionLine = 0;
}

std::string Process::getName() 
{
	return this->processName;
}

int Process::getCoreID()
{
	return this->coreID;
}

int Process::getProcessID() const
{
	return  this->pid;
}

int Process::getTotalInstructionLines() const
{
	return this->instructionList.size();
}

int Process::getCurrentInstructionLines()
{
	return this->currentInstructionLine;
}

tm* Process::getCreatedAt()
{
	return this->createdAt;
}

tm* Process::getFinishedAt()
{
	return this->finishedAt;
}

void Process::testInitFCFS()
{
	//specifically for the FCFS scheduler stuff
	std::string instruction = "Hello world from " + processName;
	for (size_t i = 0; i < 100; i++)
	{
		instructionList.push_back(instruction);
	}
}

bool Process::isDone() const
{
	if (currentInstructionLine >= instructionList.size())
	{
		return true;
	}
	else {
		return false;
	}
}

void Process::setCoreID(int coreID)
{
	this->coreID = coreID;
}

void Process::execute()
{
	std::lock_guard<std::mutex> lock(this->mtx);
	//should probably make it so that there are different things this can do
	//for now (09/10/24) it will be for printing to the outputfile
	std::time_t now = std::time(nullptr);

	// Convert time to local time (tm struct)
	std::tm* local_time = std::localtime(&now);

	// Print time in desired format: Weekday | Month | Day | HH:MM:SS | YYYY
	if (!this->isDone())
	{
		//append or create file
		std::fstream output;

		//check if file is new or old
		std::ifstream checker("./Logs/" + processName + ".txt");
		if (checker.good())
		{
			//append
			checker.close();
			output.open("./Logs/" + processName + ".txt", std::ios::out | std::ios::app);
			output << std::put_time(local_time, "(%d/%m/%Y %I:%M:%S%p) ")
				<< "Core:" << this->coreID
				<< " " << instructionList.at(currentInstructionLine) << std::endl;
			currentInstructionLine++;
			
		}
		else
		{
			//first time
			checker.close();
			output.open("./Logs/" + processName + ".txt", std::ios::out | std::ios::app);

			output << "Process name: " << processName << std::endl;
			output << "Logs: " << std::endl << std::endl;
			output << std::put_time(local_time, "(%d/%m/%Y %I:%M:%S%p) ")
				<< "Core:" << this->coreID
				<< " " << instructionList.at(currentInstructionLine) << std::endl;
			currentInstructionLine++;
			output.close();
		}
	}
	else
	{
		std::cout << "Completed!" << std::endl;
		//it's done
		std::time_t now = std::time(nullptr);
		// Convert time to local time (tm struct)
		this->finishedAt = std::localtime(&now);
	}
}