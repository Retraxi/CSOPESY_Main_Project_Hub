#include "MainConsole.h"
#include "ConsoleManager.h"
#include "CPUScheduler.h"
#include <iostream>
#include <string>

void MainConsole::printHeader() //Header function
{
	std::cout << R"(
   _,.----.    ,-,--.    _,.---._        _ __       ,----.    ,-,--.                  
 .' .' -   \ ,-.'-  _\ ,-.' , -  `.   .-`.' ,`.  ,-.--` , \ ,-.'-  _\ ,--.-.  .-,--. 
/==/  ,  ,-'/==/_ ,_.'/==/_,  ,  - \ /==/, -   \|==|-  _.-`/==/_ ,_.'/==/- / /=/_ /  
|==|-   |  .\==\  \  |==|   .=.     |==| _ .=. ||==|   `.-.\==\  \   \==\, \/=/. /   
|==|_   `-' \\==\ -\ |==|_ : ;=:  - |==| , '=',/==/_ ,    / \==\ -\   \==\  \/ -/    
|==|   _  , |_\==\ ,\|==| , '='     |==|-  '..'|==|    .-'  _\==\ ,\   |==|  ,_/     
\==\.       /==/\/ _ |\==\ -    ,_ /|==|,  |   |==|_  ,`-._/==/\/ _ |  \==\-, /      
 `-.`.___.-'\==\ - , / '.='. -   .' /==/ - |   /==/ ,     /\==\ - , /  /==/._/       
             `--`---'    `--`--''   `--`---'   `--`-----``  `--`---'   `--`-`         
   )" << std::endl;

	std::cout << "\033[32m" << "Welcome to JC Online command line!" << "\033[0m" << std::endl;
	std::cout << "\033[33m" << "Type 'exit' to quit, 'clear' to clear the screen." << "\033[0m" << std::endl;
}

void MainConsole::display()
{
	//empty for now
}

MainConsole::MainConsole() : AConsole("MAIN_CONSOLE")
{
	this->refresh = false;
}

void MainConsole::onEnabled()
{

}

void MainConsole::printProcesses()
{
	ConsoleManager::getInstance()->printScreens();
}

void MainConsole::process()
{
	if (this->refresh == false) {
		printHeader();
		this->refresh = true;
	}
	std::cout << "Enter a command:";

	std::string command;
	std::getline(std::cin, command);

	if (command == "clear" || command == "cls") {
		system("cls");
	}
	else if (command.substr(0,9) == "screen -r")
	{
		//need error checker here
		if (command.length() < 10)
		{
			std::cout << "Command incomplete." << std::endl;
		}
		else {
			ConsoleManager::getInstance()->switchToScreen(command.substr(10));
			this->refresh = false; //when exiting back will re-print the header
		}
	}
	else if (command.substr(0, 9) == "screen -s")
	{
		//make process using it for now
		std::shared_ptr<Process> newProcess = std::make_shared<Process>(ConsoleManager::getInstance()->tableSize() + 1, command.substr(10));
		std::shared_ptr<Screen> newScreen = std::make_shared<Screen>(newProcess, newProcess->getName());
		ConsoleManager::getInstance()->registerScreen(newScreen);
		std::cout << "New Screen created: " << newProcess->getName() << std::endl;
	}
	else if (command.substr(0, 10) == "screen -ls")
	{
		//std::cout << "Number of processes: " << ConsoleManager::getInstance()->tableSize() << std::endl;
		//ConsoleManager::getInstance()->printScreens();
		std::cout << "--------------------------------------------------" << std::endl;//60
		std::cout << "Running processes: " << std::endl;
		//function to print running processes
		CPUScheduler::getInstance()->printRunningProcesses();
		std::cout <<  std::endl;
		std::cout << "Finished processes: " << std::endl;
		//function to print out finished processes
		CPUScheduler::getInstance()->printFinishedProcesses();
		std::cout << "--------------------------------------------------" << std::endl;//60

	}
	else if (command.substr(0, 10) == "startFCFS") {
		CPUScheduler::getInstance()->startScheduler(1);
		std::cout << std::endl << "FCFS scheduler has started." << std::endl;
	}
	else if (command == "exit")
	{
		//close the app
		ConsoleManager::getInstance()->exitApplication();
	}
	else {
		std::cout << "Command not recognized." << std::endl;
	}
}