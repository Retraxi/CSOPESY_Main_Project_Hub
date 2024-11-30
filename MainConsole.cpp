#include "MainConsole.h"
#include "ConsoleManager.h"
#include "CPUScheduler.h"
#include <iostream>
#include "PagingAllocator.h"
#include <fstream>
#include <sstream>
#include <string>


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
//T
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
		else if (command.substr(0, 14) == "save-backstore")
		{
			// Save a snapshot of the current memory to the backing store
			size_t snapshotId = std::stoul(command.substr(15));
			PagingAllocator allocator; // Ensure PagingAllocator is initialized properly
			allocator.saveMemorySnapshot(snapshotId);
			std::cout << "Backstore snapshot saved as memory_snapshot_" << snapshotId << ".txt\n";
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

				int maxMemory = stoul(configTable["max-overall-mem"]);
				int minMPP = stoul(configTable["min-mem-per-proc"]);
				int maxMPP = stoul(configTable["max-mem-per-proc"]);

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
				//std::cout << "Initialization complete with Memory Mode: " << memoryMode
					//<< " and Backing Store Directory: " << backingStoreDir << "." << std::endl;
			}

		}
	}
}





/*


#include <cstdlib>
#include <fstream> // in use
#include <iostream>
#include <memory>
#include <sstream> // in use
#include <string>
#include <vector>

#include "AConsole.h"
#include "Config.h"
#include "ConsoleManager.h"
#include "Cpu.h"
#include "MainConsole.h"
#include "MarqueeConsole.h"
#include "MemoryManager.h"
#include "PrintCommand.h"
#include "Scheduler.h"

#define SPACE " "


MainConsole::MainConsole(ConsoleManager* conman) : AConsole("MAIN_CONSOLE"), _conman(conman) {
	// SCREEN
	this->_commandMap["screen"] = [conman](argType arguments) {
		if (arguments.size() > 2) {
			std::cout << "Too many strings!" << std::endl;
			return;
		}
		else if (arguments.at(0) == "screen") {
			std::cout << "TODO: help for screen" << std::endl;
			return;
		}

		if (arguments.at(0) == "-s") {
			if (arguments.size() == 1)
				std::cout << "No process specified." << std::endl;
			else
				conman->newConsole(arguments.at(1));
		}
		else if (arguments.at(0) == "-r") {
			if (arguments.size() == 1)
				std::cout << "No process specified." << std::endl;
			else
				conman->switchConsole(arguments.at(1));
		}
		else if (arguments.at(0) == "-ls") {
			conman->_scheduler->printStatus();
		}
		else {
			std::cout << "TODO: help for screen" << std::endl;
			return;
		}
		};
	this->_commandMap["marquee"] = [conman](argType arguments) {
		conman->switchConsole("MARQUEE_CONSOLE");
		};
	this->_commandMap["report-util"] = [conman](argType arguments) {
		std::ofstream out("csopesy-log.txt");
		std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
		std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
		conman->_scheduler->printStatus();
		std::cout.rdbuf(coutbuf); //reset to standard output again
		std::cout << "root:\\> Report generated at root:/csopesy-log.txt\n";
		};
	this->_commandMap["scheduler-test"] = [conman](argType arguments) {
		conman->_scheduler->schedulerTest();
		};
	this->_commandMap["scheduler-stop"] = [conman](argType arguments) {
		conman->_scheduler->schedulerTestStop();
		};
	this->_commandMap["memory"] = [conman](argType arguments) {
		conman->_scheduler->printMem();
		};
	this->_commandMap["process-smi"] = [conman](argType arguments) {
		conman->_scheduler->processSmi();
		};
	this->_commandMap["vmstat"] = [conman](argType arguments) {
		conman->_scheduler->vmstat();
		};
}

void MainConsole::run() {
	std::string input;
	while (!this->_initialized) {
		std::cout << "root:\\> ";
		getline(std::cin, input);
		if (input == "initialize") {
			this->_initialized = true;
			this->_conman->newConsole("MARQUEE_CONSOLE", std::make_shared<MarqueeConsole>(144));

			Config config = Config();
			config.initialize();

			if (config.getMinPageProc() != 1 && config.getMaxPageProc() != 1) {
				Process::setRequiredMemory(config.getMinMemProc(), config.getMaxMemProc());
			}

			Scheduler::initialize(config.getNumCpu(),
				config.getBatchProcessFreq(),
				config.getMinIns(), config.getMaxIns(),
				config.getMinMemProc(), config.getMaxMemProc(),
				config.getMaxMem(), config.getMinPageProc(), config.getMaxPageProc());

			Scheduler* sched = Scheduler::get();

			this->_conman->_scheduler = sched;

			PrintCommand::setMsDelay(0);
			CPU::setMsDelay(config.getDelaysPerExec() * 100);

			std::string schedType = config.getScheduler();
			if (schedType == "fcfs") {
				sched->startFCFS(config.getDelaysPerExec());
			}
			else if (schedType == "sjf") {
				sched->startSJF(config.getDelaysPerExec(), config.isPreemptive());
			}
			else if (schedType == "rr") {
				sched->startRR(config.getDelaysPerExec(), config.getQuantumCycle());
			}

		}
		if (input == "exit") {
			this->stop();
			return;
		}
	}
	system("cls");
	this->_active = true;
	this->printHeader();
	while (this->_active) {
		std::cout << "root:\\> ";
		std::getline(std::cin, input);
		std::string command = input.substr(0, input.find(SPACE));
		input.erase(0, input.find(SPACE) + 1);
		if (command == "exit") {
			this->stop();
			return;
		}
		if (this->_commandMap.find(command) == this->_commandMap.end())
			std::cout << "'" << command << "' is not recognized." << std::endl;
		else {
			std::stringstream tokens(input);
			std::vector<std::string> arguments;
			std::string token;
			while (std::getline(tokens, token, ' ')) {
				arguments.push_back(token);
			}
			this->_commandMap[command](arguments);
		}
	}
}

void MainConsole::stop() {
	this->_active = false;
}

void MainConsole::draw() {
}

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



*/
