#include "Screen.h"
#include "ConsoleManager.h"
#include <iostream>
#include <filesystem>  


void Screen::printProcessInfo() const
{
	//get process data and print it out
	std::cout << this->attachedProcess->getName() << std::endl;

	std::cout << "Process ID: " << this->attachedProcess->getProcessID() << std::endl;

	std::cout << "Current instruction line: " << this->attachedProcess->getCurrentInstructionLines() << std::endl;

	std::cout << "Total instruction lines: " << this->attachedProcess->getTotalInstructionLines() << std::endl;
}


Screen::Screen(std::shared_ptr<Process> process, String processName) : AConsole(processName)
{
	this->refreshed = false;
	this->attachedProcess = process;
}

void Screen::onEnabled()
{
	//put something here
}

void Screen::process()
{
	if (this->refreshed == false) {
		this->refreshed = true;
		this->printProcessInfo();
	}

	std::cout << "root:\\>";

	std::string command;
	std::getline(std::cin, command);

	if (command == "clear" || command == "cls") {
		system("cls");
	}
	else if (command == "process-smi")
	{
		this->printProcessInfo();
	}
	else if (command == "exit")
	{
		system("cls");
		ConsoleManager::getInstance()->returnToPreviousConsole();
		ConsoleManager::getInstance()->unregisterScreen(this->name);
	}
	else {
		std::cout << "Command not recognized." << std::endl;
	}
}

void Screen::display()
{
	    std::cout << "Screen " << this->name << " is now active." << std::endl;

}