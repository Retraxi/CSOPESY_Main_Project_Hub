#include "MainConsole.h"
#include "ConsoleManager.h"
#include "CPUScheduler.h"
#include <iostream>
#include <fstream>
#include <sstream>
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

	if (this->initialized) {
		if (command == "clear" || command == "cls") {
			system("cls");
		}
		else if (command.substr(0, 9) == "screen -r")
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
			CPUScheduler::getInstance()->printCoreInfo();
			std::cout << "--------------------------------------------------" << std::endl;//60
			std::cout << "Running processes: " << std::endl;
			//function to print running processes
			CPUScheduler::getInstance()->printRunningProcesses();
			std::cout << std::endl;
			std::cout << "Finished processes: " << std::endl;
			//function to print out finished processes
			CPUScheduler::getInstance()->printFinishedProcesses();
			std::cout << "--------------------------------------------------" << std::endl;//60

		}
		else if (command.substr(0, 14) == "scheduler-test")
		{
			if (CPUScheduler::getInstance()->getTest() == true) {
				std::cout << "Scheduler is already generating processes." << std::endl;
			}
			else {
				CPUScheduler::getInstance()->setTest(true);
				std::cout << "Scheduler will now begin generating processes." << std::endl;
			}

		}
		else if (command.substr(0, 14) == "scheduler-stop")
		{
			if (CPUScheduler::getInstance()->getTest() == true) {
				CPUScheduler::getInstance()->setTest(false);
				std::cout << "Scheduler will stop generating processes." << std::endl;
			}
			else {

				std::cout << "Scheduler is not generating processes at the moment." << std::endl;
			}

		}
		else if (command.substr(0, 10) == "startFCFS") {
			CPUScheduler::getInstance()->startScheduler("fcfs");
			std::cout << std::endl << "FCFS scheduler has started." << std::endl;
		}
		else if (command == "report-util") {
			CPUScheduler::getInstance()->generateReport();
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
	else {
		if (command != "initialize") {
			std::cout << "[WARNING]: Process configuration has not yet been initialized! Please type \"initialize\"." << std::endl;
		}
		else if (command == "initialize") {
			//read from config and everything
			std::ifstream reader("config.txt", std::ios::in);
			if (!reader.good()) {
				//warn
				reader.close();
				std::cout << "[WARNING]: The file being opened doesn't exist or is corrupted." << std::endl;
			}
			else {
				std::unordered_map<std::string, std::string> configTable;

				//start reading the file
				//format uses key value pairs
				std::string line;
				std::string key;
				std::string value;
				while (getline(reader, line)) {
					std::istringstream lineReader(line);
					lineReader >> key >> value;
					if (value == "0") {
						value = "0";
					}
					configTable[key] = value;
				}

				reader.close();

				//std::cout << configTable["num-cpu"] << std::endl;
				//initialize the CPU Scheduler using the read data
				//it didn't work when I put it directly so I'm assigning it to some values
				int cpuNum = stoi(configTable["num-cpu"]);
				unsigned int qCycle = stoul(configTable["quantum-cycles"]);
				unsigned int bpFreq = stoul(configTable["batch-process-freq"]);
				unsigned int miIns = stoul(configTable["min-ins"]);
				unsigned int maIns = stoul(configTable["max-ins"]);
				unsigned int dpEx = 0; //if it's somehow empty

				if (configTable["delays-per-exec"] == "\0")
				{
					dpEx = 0;
				}
				else {
					unsigned int dpEx = stoul(configTable["delays-per-exec"]);
				}

				CPUScheduler::initialize(
					cpuNum, configTable["scheduler"].substr(1, configTable["scheduler"].size() - 1),
					qCycle, bpFreq,
					miIns, maIns,
					dpEx
				);

				this->initialized = true;
				std::cout << "Initialization complete." << std::endl;
			}

		}
	}
}