#include "Screen.h"
#include "ConsoleManager.h"
#include <iostream>

void Screen::printProcessInfo() const
{
	//get process data and print it out
	std::cout << this->attachedProcess->getName() << std::endl;

	std::cout << "Process ID: " << this->attachedProcess->getProcessID() << std::endl;

	std::cout << "Current instruction line: " << this->attachedProcess->getCurrentInstructionLine() << std::endl;

	std::cout << "Total instruction lines: " << this->attachedProcess->getTotalInstructionLines() << std::endl;
}


Screen::Screen(std::shared_ptr<Process> process, String processName) : AConsole(processName)
{
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
		ConsoleManager::getInstance()->returnToPreviousConsole();
		ConsoleManager::getInstance()->unregisterScreen(this->name);
	}
	else {
		std::cout << "Command not recognized." << std::endl;
	}
}

void Screen::display()
{
	//not sure about this
	printProcessInfo();
}