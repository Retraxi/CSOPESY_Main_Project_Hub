#define _CRT_SECURE_NO_WARNINGS
#include "Process.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
Process::Process(int pid, std::string name) : pid(pid), processName(name)
{
	this->coreID = -1;
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

void Process::setFinishedAt(tm* finishedAt)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	this->finishedAt = finishedAt;
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

void Process::setInstruction(int totalCount) {
	std::string instruction = "Hello world from " + processName;
	for (size_t i = 0; i < totalCount; i++)
	{
		instructionList.push_back(instruction);
	}
}

bool Process::isDone()
{
	if (currentInstructionLine >= instructionList.size())
	{
		//std::cout << processName << " has finished!";
		//it's done
		std::time_t now = std::time(nullptr);
		// Convert time to local time (tm struct)
		setFinishedAt(std::localtime(&now));
		return true;
	}
	else {
		return false;
	}
}

void Process::setCoreID(int coreID)

{
	std::lock_guard<std::mutex> lock(this->mtx);
	this->coreID = coreID;
}


bool Process::loadFromBackingStore() {
    std::ifstream file("./BackingStore/process_" + std::to_string(pid) + ".txt");

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Process ID:") != std::string::npos) {
                pid = std::stoi(line.substr(line.find(":") + 2));
            } else if (line.find("Process Name:") != std::string::npos) {
                processName = line.substr(line.find(":") + 2);
            } else if (line.find("Current Instruction Line:") != std::string::npos) {
                currentInstructionLine = std::stoi(line.substr(line.find(":") + 2));
            } else if (line.find("Core ID:") != std::string::npos) {
                coreID = std::stoi(line.substr(line.find(":") + 2));
            } else if (line.find("Created At:") != std::string::npos) {
                std::string createdAtStr = line.substr(line.find(":") + 2);

                // Convert the string to a std::tm object
                std::tm tm = {};
                std::istringstream ss(createdAtStr);
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");  // Assuming the date format is "2024-11-28 15:30:00"

                if (ss.fail()) {
                    std::cerr << "Failed to parse time string: " << createdAtStr << std::endl;
                } else {
                    // Convert std::tm to time_t
                    std::time_t createdAtTime = std::mktime(&tm);
                    createdAt = std::localtime(&createdAtTime);
                }
            } else if (line.find("Finished At:") != std::string::npos) {
                std::string finishedAtStr = line.substr(line.find(":") + 2);

                // Convert the string to a std::tm object
                std::tm tm = {};
                std::istringstream ss(finishedAtStr);
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");  // Assuming the date format is "2024-11-28 15:30:00"

                if (ss.fail()) {
                    std::cerr << "Failed to parse time string: " << finishedAtStr << std::endl;
                } else {
                    // Convert std::tm to time_t
                    std::time_t finishedAtTime = std::mktime(&tm);
                    finishedAt = std::localtime(&finishedAtTime);
                }
            }
        }

        file.close();
        std::cout << "Process " << pid << " loaded from backing store." << std::endl;
        return true;
    } else {
        std::cerr << "Error: Unable to open file for process " << pid << std::endl;
        return false;
    }
}


void Process::execute()
{
	//std::lock_guard<std::mutex> lock(this->mtx);
	//should probably make it so that there are different things this can do
	//for now (09/10/24) it will be for printing to the outputfile
	std::time_t now = std::time(nullptr);

	// Convert time to local time (tm struct)
	std::tm* local_time = std::localtime(&now);
	int dummy = 0;

	// Print time in desired format: Weekday | Month | Day | HH:MM:SS | YYYY
	if (!this->isDone())
	{

		//expensive operation or delay
		//std::this_thread::sleep_for(std::chrono::microseconds(10));

		//or a long for loop
		for (size_t i = 0; i < 10000; i++)
		{
			dummy += 1;
		}
		currentInstructionLine++;

		//append or create file
		/*
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
		*/
	}
}