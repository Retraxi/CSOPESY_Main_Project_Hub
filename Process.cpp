#include "Process.h"


std::string Process::getName() 
{
	return this->processName;
}

int Process::getProcessID() const
{
	return  this->pid;
}

int Process::getCurrentInstructionLine() const
{
	return this->currentInstructionLine;
}

int Process::getTotalInstructionLines() const
{
	return this->totalInstructionLines;
}

Process::Process(int pid, std::string name) : pid(pid), processName(name)
{
	this->currentInstructionLine = 0;
	this->totalInstructionLines = 50;
}