#include "MainConsole.h"
#include "ConsoleManager.h"
#include "CPUScheduler.h"
#include <iostream>
#include "PagingAllocator.h"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

void MainConsole::printHeader() // Header function
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

    std::cout << "\033[32m"
        << "Welcome to JC Online command line!"
        << "\033[0m" << std::endl;
    std::cout << "\033[33m"
        << "Type 'exit' to quit, 'clear' to clear the screen."
        << "\033[0m" << std::endl;
}

void MainConsole::display()
{
    // empty for now
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
    if (!this->refresh) {
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
        else if (command.substr(0, 9) == "screen -r") {
            if (command.length() < 10) {
                std::cout << "Command incomplete." << std::endl;
            }
            else {
                ConsoleManager::getInstance()->switchToScreen(command.substr(10));
                this->refresh = false;
            }
        }
        else if (command.substr(0, 9) == "screen -s") {
            std::shared_ptr<Process> newProcess = std::make_shared<Process>(ConsoleManager::getInstance()->tableSize() + 1, command.substr(10));
            std::shared_ptr<Screen> newScreen = std::make_shared<Screen>(newProcess, newProcess->getName());
            ConsoleManager::getInstance()->registerScreen(newScreen);
            std::cout << "New Screen created: " << newProcess->getName() << std::endl;
        }
        else if (command.substr(0, 10) == "screen -ls") {
            CPUScheduler::getInstance()->printCoreInfo();
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << "Running processes: " << std::endl;
            CPUScheduler::getInstance()->printRunningProcesses();
            std::cout << std::endl;
            std::cout << "Finished processes: " << std::endl;
            CPUScheduler::getInstance()->printFinishedProcesses();
            std::cout << "--------------------------------------------------" << std::endl;
        }
        else if (command.substr(0, 14) == "scheduler-test") {
            if (CPUScheduler::getInstance()->getTest()) {
                std::cout << "Scheduler is already generating processes." << std::endl;
            }
            else {
                CPUScheduler::getInstance()->setTest(true);
                std::cout << "Scheduler will now begin generating processes." << std::endl;
            }
        }
        else if (command.substr(0, 14) == "scheduler-stop") {
            if (CPUScheduler::getInstance()->getTest()) {
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
        else if (command == "exit") {
            ConsoleManager::getInstance()->exitApplication();
        }
        else {
            std::cout << "Command not recognized." << std::endl;
        }
    }
    else if (command == "initialize") {
        std::ifstream reader("config.txt", std::ios::in);
        if (!reader.good()) {
            std::cout << "[WARNING]: The file being opened doesn't exist or is corrupted." << std::endl;
            return;
        }

        std::unordered_map<std::string, std::string> configTable;
        std::string line, key, value;

        // Read configuration
        while (getline(reader, line)) {
            std::istringstream lineReader(line);
            lineReader >> key >> value;
            configTable[key] = value;
        }

        reader.close();

        try {
            int cpuNum = std::stoi(configTable.at("num-cpu"));
            unsigned int qCycle = std::stoul(configTable.at("quantum-cycles"));
            unsigned int bpFreq = std::stoul(configTable.at("batch-process-freq"));
            unsigned int miIns = std::stoul(configTable.at("min-ins"));
            unsigned int maIns = std::stoul(configTable.at("max-ins"));
            unsigned int dpEx = std::stoul(configTable.at("delay-per-exec"));
            unsigned int maxMemory = std::stoul(configTable.at("max-overall-mem"));
            unsigned int memPerFrame = std::stoul(configTable.at("mem-per-frame"));
            unsigned int memPerProc = std::stoul(configTable.at("mem-per-proc"));
            std::string schedulerType = configTable.at("scheduler");
            if (schedulerType.front() == '"') schedulerType = schedulerType.substr(1, schedulerType.size() - 2);

            CPUScheduler::initialize(
                cpuNum, schedulerType, qCycle, bpFreq,
                miIns, maIns, dpEx,
                maxMemory, memPerProc, memPerProc, memPerFrame
            );

            this->initialized = true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error initializing configuration: " << e.what() << std::endl;
        }
    }
}
